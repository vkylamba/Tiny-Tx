#define TX_PIN 0 // PB0

void sendBit(bool bitVal) {
  digitalWrite(TX_PIN, bitVal);
  delayMicroseconds(400); // ~1200 bps
}

void sendByte(uint8_t b) {
  sendBit(1); // Start bit
  for (int i = 0; i < 8; i++) {
    sendBit(b & 1);
    b >>= 1;
  }
  sendBit(0); // Stop bit
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

void sendRFRawMessage(const char* msg) {
  while (*msg) sendByte(*msg++);
  uint16_t crc = crc16(msg);
  sendByte((crc >> 8) & 0xFF); // High byte
  sendByte(crc & 0xFF);        // Low byte
}

void setupRFRaw() {
  pinMode(TX_PIN, OUTPUT);
}
