#include <stdint.h>
#include <Arduino.h>
#include <util/crc16.h>

#define RESEND_COUNT 1
#define RESEND_DELAY 150

#define RADIO_BAUD_ADJUST -15  // experimentally
#define RADIO_DELAY       100  // experimentally -- 100 uSec delay before transmitter starts

/* Miniature VirualWire/RadioHead RH_ASK implementation adapted for running
 * on an ATTiny85.
 */
#define TX_BAUDRATE       1200
#define TX_MICROINTERVAL  (1000000UL/TX_BAUDRATE)
#define TX_MAXLEN         12

#define RADIO_TX          0

uint32_t vw_microinterval_main = TX_MICROINTERVAL;
uint32_t vw_microinterval_advance = 0UL;

uint8_t vw_lastbit = 0;

inline void vw_txbit(const bool onOff) { digitalWrite(RADIO_TX, (vw_lastbit = onOff) ? HIGH : LOW); }

/* These should be PROGMEM or else just generated.  This way they waste too much RAM.
 */
const uint8_t vw_header[8] = { 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x38, 0x2C };
const uint8_t vw_conv4to6[16] = { 0x0D, 0x0E, 0x13, 0x15, 0x16, 0x19, 0x1A, 0x1C,
                               0x23, 0x25, 0x26, 0x29, 0x2A, 0x2C, 0x32, 0x34 };

/* Here we transmit the sequence of bits.  Generally, we can simply set the transmitter on or off
 * then wait for the fundamental but timing and go on to the next bit.  But the radios often have
 * a significant startup time that affects the timing of an OFF-->ON transition.  In order to 
 * compensate for this timing glitch, we have to shift the timing of these transitions and therefore
 * have to know the type of transition occurring.
 * 
 * In order to accommodate the timing shift, the timing delay is placed BEFORE the bit transition.
 * This is a little unnatural.
 * 
 */
void vw_rawSend(const uint8_t * p, uint8_t len)
{
  while (len--) {
    const uint8_t val = *p++;
    for (uint8_t mask = 1; mask != 0x40; mask <<= 1) {

      /* If the transition is OFF-->ON, then we do weird timing.
       */
      if (!vw_lastbit && (val & mask)) {
        delayMicroseconds(vw_microinterval_main);
        vw_txbit(val & mask);
        delayMicroseconds(vw_microinterval_advance);

      /* Otherwise, this is the normal case.
       */
      } else {
        delayMicroseconds(vw_microinterval_main);
        delayMicroseconds(vw_microinterval_advance);
        vw_txbit(val & mask);
      }
    }
  }
}

/* Allow for ajustment of the delay timing in uS.  This is
 * needed to compensate for timing inaccuracies.  The main 
 * delta alters the overall bit timing whichis generally 
 * affected by the clock speed.  "on_delay" is a value that 
 * specifically compensates for the turn on delay of the 
 * radio transmitter.  This value has to  be measured with 
 * a specific device and typical values might be in the 
 * 5..65 uS range.  Radios with small delays don't necessarily
 * need compensation, but ones with long delays will affect
 * receive accuracy unless we alter the software timing to
 * compensate.
 */
uint32_t vw_set_microinterval(int delta, int on_delay)
{
   vw_microinterval_main = TX_MICROINTERVAL;
   vw_microinterval_main += (long) delta;
   vw_microinterval_advance = on_delay;
   vw_microinterval_main -= on_delay;
   return(vw_microinterval_main + vw_microinterval_advance);
}

/* Send a buffer using the virtual wire OOK protocol
 */
void vw_send_buf(const uint8_t * buf, const uint8_t len)
{
  /*  First create the payload byte stream by converting to 6:4 format
   *  compute the CRC and append that.
   */
  static uint8_t payload[(TX_MAXLEN+3)*2];
  uint8_t * p = payload;
  uint16_t crc = 0xFFFF;
  uint8_t v = len + 3;
  crc = _crc_ccitt_update(crc, v);
  *p++ = vw_conv4to6[v >> 4];
  *p++ = vw_conv4to6[v & 0x0F];
  for (uint8_t l = len; l--;) {
    v = *buf++;
    crc = _crc_ccitt_update(crc, v);
    *p++ = vw_conv4to6[v >> 4];
    *p++ = vw_conv4to6[v & 0x0F];
  }
  crc = ~crc;
  v = (uint8_t)crc;
  *p++ = vw_conv4to6[v >> 4];
  *p++ = vw_conv4to6[v & 0x0F];
  v = (uint8_t)(crc >> 8);
  *p++ = vw_conv4to6[v >> 4];
  *p++ = vw_conv4to6[v & 0x0F];

  /*  Now transmit the header and the encoded payload 
   *  turn off the transmitter.  Note that the timing delay
   *  is BEFORE the bit being sent, so an advance timing delay
   *  is appended before the final radio OFF.
   */
  vw_rawSend(vw_header, sizeof(vw_header));
  vw_rawSend(payload, (len + 3)*2);
  delayMicroseconds(vw_microinterval_main);
  delayMicroseconds(vw_microinterval_advance);
  vw_txbit(0);
}

void sendRFRawMessage(const char *msg) {
    uint16_t size = strlen(msg);
    // vw_set_microinterval(RADIO_BAUD_ADJUST, RADIO_DELAY);
    for (uint8_t i = 0; i < RESEND_COUNT; i++) {
        if (i != 0) delay(RESEND_DELAY);
        vw_send_buf((uint8_t*)msg, size);
    }
}