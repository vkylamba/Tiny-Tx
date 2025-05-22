# Minimal, Low-Power Energy Meter Schematic

Designed for:

- **ATtiny85** (3.3 V, running at 8 MHz)
- Powered by a 3.7 V Li-ion cell with LDO
- Voltage measurement (bidirectional AC/DC)
- Current sensing using WCS1500 with LM358
- FS1000A ASK RF transmitter
- EEPROM-backed energy tracking
- No LCD/LED, no charging, minimal footprint

---

## ⚡ Energy Meter – Schematic (ASCII Art)

```
Power Supply:
  [Li-ion Battery 3.7V] | [LDO 3.3V]
         +--+--+--------------+
         |  |                 |
        GND VCC           [ATtiny85] (3.3V, 8MHz)
                             +---+---+
                             |   |
                      [ADC Pins] [FS1000A TX Data Pin]

Voltage Measurement (Bi-directional AC/DC):
  [Vin+] ----+----[Voltage Divider R1]----+
             |                            |
      [Voltage Divider R2]                |
             |                            |
    +----[Protection Diode]-----+----> ATtiny85 ADC (e.g., A0)
    |                           |
   GND                         GND

Current Measurement (WCS1500 Hall Sensor):
  AC/DC Load Line ----> [WCS1500 Sensor] ----> [LM358 Amp] ----> ATtiny85 ADC (e.g., A1)

ASK RF Transmission:
  ATtiny85 GPIO (e.g., D3) ---> [FS1000A DATA]
  VCC and GND shared from LDO

EEPROM:
  - Use internal EEPROM on ATtiny85 to store energy_Wh
  - Save periodically or when significant change detected

Battery:
  - 1x 3.7 V Li-ion (with protection)
  - Output: ~4.2V max → Regulated to 3.3 V by LDO (MCP1700 or HT7333)
```

---

## 🧩 Component Summary (BOM)

| Component        | Value / Part No.         | Notes                          |
|------------------|-------------------------|--------------------------------|
| ATtiny85         | -                       | 3.3 V, internal 8 MHz          |
| LDO Regulator    | MCP1700-3302 / HT7333   | Low-dropout, low quiescent     |
| Capacitors       | 1–10 µF                 | Input/output filter for LDO    |
| FS1000A TX       | ASK RF Transmitter      | 3.3 V compatible               |
| WCS1500          | Current sensor          | ±15A, analog output            |
| LM358            | Op-amp                  | Used to scale WCS1500 output   |
| Resistors        | R1, R2 for divider      | Tune to keep Vout < 3.3 V      |
| Diode            | 1N4148 or similar       | Over-voltage clamp             |
| EEPROM           | Internal                | Store cumulative energy        |
| Battery          | 3.7 V Li-ion            | Replaceable or protected cell  |

