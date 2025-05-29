#include <Arduino.h>
#define TX_PIN 0 // PB0

#define PREAMBLE_SIZE 8 // Number of preamble bytes
#define PREAMBLE_BYTE 0xAA
#define START_BYTE1 0x2D
#define START_BYTE2 0xD4
#define STOP_BYTE 0xD2

#define MESSAGE_SIZE 98
#define REPEAT 2 // Number of times to repeat the message

#define BAUD_RATE 100 // Baud rate for transmission
#define WAIT_TIME 1000000L / BAUD_RATE
#define BIT_DURATION WAIT_TIME / 2 // As with the Manchester encoding we send 2 pulses per bit


void sendBitManchester(bool bitVal) {
  // Manchester encoding: 0 = LOW->HIGH, 1 = HIGH->LOW
  if (bitVal) {
    digitalWrite(TX_PIN, HIGH);
    delayMicroseconds(BIT_DURATION);
    digitalWrite(TX_PIN, LOW);
    delayMicroseconds(BIT_DURATION);
  } else {
    digitalWrite(TX_PIN, LOW);
    delayMicroseconds(BIT_DURATION);
    digitalWrite(TX_PIN, HIGH);
    delayMicroseconds(BIT_DURATION);
  }
}

void sendByte(uint8_t b) {
  for (int i = 0; i < 8; i++) {
    sendBitManchester(b & 1);
    b >>= 1;
  }
}

uint16_t crc16(const char* data) {
  uint16_t crc = 0xFFFF;
  while (*data) {
    crc ^= (*data++) << 8;
    for (uint8_t i = 0; i < 8; i++) {
      crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
    }
  }
  return crc;
}

void sendPreamble() {
  for (int i = 0; i < PREAMBLE_SIZE; i++) {
    sendByte(PREAMBLE_BYTE);
  }
  sendByte(START_BYTE1);
  sendByte(START_BYTE2);
}

void sendRFMessageRaw(const char* msg, uint16_t msgLength, uint16_t crc) {
  digitalWrite(TX_PIN, LOW); // Ensure TX pin is low before sending
  delay(100); // Allow time for the transmitter to stabilize
  sendPreamble(); // Send preamble
  sendByte(msgLength); // Send message length (low byte)
  for (uint16_t i = 0; i < msgLength; i++) {
    sendByte(msg[i]);
  }
  sendByte((crc >> 8) & 0xFF); // High byte
  sendByte(crc & 0xFF);        // Low byte
  sendByte(STOP_BYTE); // Send stop byte
  delayMicroseconds(100); // Wait for a short time after sending
  digitalWrite(TX_PIN, LOW); // Ensure TX pin is low after sending
}

void setupRFRaw() {
  pinMode(TX_PIN, OUTPUT);
}

void sendRFRawMessage(const char* msg) {
  uint16_t crc = crc16(msg);
  uint16_t msgLength = strlen(msg);
  for(int i = 0; i < REPEAT; i++) {
    sendRFMessageRaw(msg, msgLength, crc); // Send start byte
    delay(100); // Wait before sending the next message
  }
}