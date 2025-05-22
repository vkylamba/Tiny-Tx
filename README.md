# 🧪 Low-Power One-Way Wireless Energy Meter

This is a **minimalist, low-power, single-channel energy meter** designed for **AC/DC voltage and current monitoring**, using:

- **ATtiny85 microcontroller** (3.3 V, 8 MHz)
- **WCS1500 Hall-effect sensor** for current
- **Voltage divider** for high-voltage AC/DC measurement
- **FS1000A ASK RF module** for one-way wireless transmission (100m+ range)
- **3.7 V Li-ion battery** with **LDO regulator**
- **EEPROM storage** for persistent energy tracking

> 🛠️ The entire system is designed for long-term deployment with replaceable battery, no LCD, no charging, and ultra-low power use.

---

## 🔧 Features

- Measures **bidirectional voltage and current** (AC/DC)
- Calculates **power and cumulative energy (Wh)**
- Transmits data wirelessly every 5 minutes (ASK, 433 MHz)
- Stores total energy in **internal EEPROM**
- Powered by a single **Li-ion battery** with **3+ months of battery life**
- Fully open-source and easily modifiable

---

## 🧱 Hardware Used

| Component        | Description                         |
|------------------|-------------------------------------|
| ATtiny85         | Microcontroller (3.3 V @ 8 MHz)     |
| FS1000A          | ASK RF transmitter module           |
| WCS1500          | Hall-effect current sensor          |
| LM358            | Op-amp for scaling current signal   |
| MCP1700 / HT7333 | LDO regulator for 3.3 V output      |
| Voltage Divider  | Resistors + diode for voltage input |
| 3.7 V Li-ion     | Battery (replaceable)               |

---

## 📦 Directory Structure

```
energy-meter/
├── src/
│   └── main.cpp         # Firmware source code
├── include/
│   └── config.h         # Measurement + transmission config
├── platformio.ini       # PlatformIO build configuration
├── schematic.md         # Circuit overview in Markdown
├── README.md            # This file
└── LICENSE              # MIT License
```

---

## 🚀 Getting Started

1. Install [PlatformIO](https://platformio.org/) in VSCode  
2. Connect your **ATtiny85** via USB programmer (e.g. USBasp or USBtinyISP)  
3. Clone this repo and run:

   ```bash
   pio run
   pio upload
   ```

4. Receiver (FS1000A) should be tuned to 433 MHz and decode message format.

---

## 🧠 Data Format

Each packet includes:

```
<ID>,<Voltage>,<Current>,<Power>,<Energy_Wh>
```

Example:

```
nodeA,230.5,1.2,276.6,58.32
```

---

## 📜 License

MIT License

```
MIT License

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```