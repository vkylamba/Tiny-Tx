#define F_CPU 8000000UL
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include "rf-raw.h"

#define DEVICE_ID "A1"
#define VOLTAGE_PIN A3
#define CURRENT_PIN A2
#define BATT_PIN A0
#define TX_PIN 0
#define LED_PIN 1

#define SAMPLES 128
#define SLEEP_CYCLES 2 // 8s × 1 ≈ 8 seconds

volatile bool watchdogFired = false;

float adcToVoltage(int adc)
{
    float v = adc * (5.0 / 1024.0);
    return (v - 2.5) * (1000.0 / 2.5);
}

float adcToCurrent(int adc)
{
    float v = adc * (5.0 / 1024.0);
    return (v - 2.5) / 1.5;
}

float computeRMS(int pin, bool isVoltage)
{
    long sumSq = 0;
    for (int i = 0; i < SAMPLES; i++)
    {
        int adc = analogRead(pin);
        float val = isVoltage ? adcToVoltage(adc) : adcToCurrent(adc);
        sumSq += val * val;
        delayMicroseconds(200);
    }
    return sqrt(sumSq / float(SAMPLES));
}

float readBatteryVoltage()
{
    int adc = analogRead(BATT_PIN);
    return (adc * 5.0 / 1024.0) * 2.0; // Assuming 2:1 divider
}

void setupRF() {
    setupRFRaw();
}

void sendMessage(const char *msg)
{
    sendRFRawMessage(msg);
}

void goToSleep()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu(); // Sleep until WDT fires
    sleep_disable();
}

ISR(WDT_vect)
{
    watchdogFired = true;
}

void setupWatchdog() {
  cli();
  MCUSR &= ~(1 << WDRF);
  WDTCR |= (1 << WDCE) | (1 << WDE);
  WDTCR = (1 << WDP3) | (1 << WDP0);  // ~8s
  WDTCR |= (1 << WDIE);               // Interrupt only
  sei();
}

float energy_Wh = 0;
float lastSavedEnergy = 0.0;
unsigned long lastCalc = 0;

void loadEnergyFromEEPROM() {
  EEPROM.get(0, energy_Wh);
  if (isnan(energy_Wh) || energy_Wh < 0 || energy_Wh > 1e6) {
    energy_Wh = 0.0;  // Fallback in case of garbage
  }
}

void saveEnergyToEEPROM() {
  EEPROM.put(0, energy_Wh);
}

void errorBlink(uint8_t code, bool isError = true, uint16_t delayTime = 300) {
    if (code < 10) {
        delayTime = isError ? 500 : 300;
        code = code * 3;
    } else  {
        code = code % 10; // Limit to 0-9 for blink count
        delayTime = delayTime * 8;
    }
    for (uint8_t i = 0; i < code; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(delayTime);
        digitalWrite(LED_PIN, LOW);
        delay(delayTime);
    }
}

void useInternal8MHz() {
#if defined(__AVR_ATtiny85__)
    CLKPR = (1 << CLKPCE); // Enable change
    CLKPR = 0;             // Set prescaler to 1 (8 MHz)
#endif
}

void setup()
{
    useInternal8MHz(); // Use internal 8MHz clock
    analogReference(DEFAULT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    delay(1000); // Allow power/oscillator to stabilize
    errorBlink(1);
    setupRF();
    setupWatchdog();
    loadEnergyFromEEPROM();
    errorBlink(1, false);
}

char msg[100];

void loop()
{

    errorBlink(1); // All good, blink error code 1s
    float Vrms = computeRMS(VOLTAGE_PIN, true);
    float Irms = computeRMS(CURRENT_PIN, false);
    float power = Vrms * Irms;

    unsigned long now = millis();
    float dt_h = (now - lastCalc) / 3600000.0;
    lastCalc = now;
    energy_Wh += dt_h > 0 ? power * dt_h : 1;

    float battV = readBatteryVoltage();
    // snprintf(msg, 100, "Hello World!");
    // sendMessage(msg);
    snprintf(msg, sizeof(msg),
             "ID:%s V:%.0f I:%.3f P:%.1f E:%.1f B:%.2f",
             DEVICE_ID, Vrms, Irms, power, energy_Wh, battV);

    sendMessage(msg);
    if (abs(energy_Wh - lastSavedEnergy) > 0.1) {
        saveEnergyToEEPROM();
        lastSavedEnergy = energy_Wh;
    }
    // Sleep ~5 minutes using watchdog in 8s chunks
    for (int i = 0; i < SLEEP_CYCLES; i++)
    {
        watchdogFired = false;
        goToSleep();
        while (!watchdogFired)
            ; // wait for ISR to set flag
    }
}
