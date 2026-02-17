#include "MPR121_Touch.h"
#include <Wire.h>
#include "MIDI.h"
#include "Config.h"

#define MPR121_ADDR 0x5A

// ================= STATE =================
static bool touchA = false;
static bool touchB = false;
static bool lastTouchA = false;
static bool lastTouchB = false;

static uint16_t refA = 0;
static uint16_t refB = 0;
static uint8_t cntA = 0;
static uint8_t cntB = 0;

// ================= I2C ===================
static void wr(uint8_t r, uint8_t v) {
  Wire.beginTransmission(MPR121_ADDR);
  Wire.write(r);
  Wire.write(v);
  Wire.endTransmission();
}

static uint16_t rd16(uint8_t r) {
  Wire.beginTransmission(MPR121_ADDR);
  Wire.write(r);
  Wire.endTransmission(false);
  Wire.requestFrom(MPR121_ADDR, 2);
  if (Wire.available() != 2) return 0;
  return Wire.read() | (Wire.read() << 8);
}

// ================= INIT ==================
void initMPR121() {

  Wire.begin();

  wr(0x5E, 0x00); // stop electrodes
  delay(10);

  // umiarkowana czułość – PROX
  wr(0x5C, 0x0A);
  wr(0x5D, 0x08);

  // wolne filtry
  wr(0x2B, 0x04);
  wr(0x2C, 0x04);
  wr(0x2D, 0x02);
  wr(0x2E, 0x02);

  // progi (nieistotne w prox)
  wr(0x41, 12);
  wr(0x42, 8);

  delay(400);

  // E0 + E1 + baseline tracking
  wr(0x5E, 0x82);
  delay(800);
}

// ================= MIDI TOUCH ============
static void sendTouchMidi() {

  // --- JOG A ---
  if (touchA && !lastTouchA)
    sendNote(MIDI_CH_A, 120, 127);
  else if (!touchA && lastTouchA)
    sendNote(MIDI_CH_A, 120, 0);

  // --- JOG B ---
  if (touchB && !lastTouchB)
    sendNote(MIDI_CH_B, 121, 127);
  else if (!touchB && lastTouchB)
    sendNote(MIDI_CH_B, 121, 0);

  lastTouchA = touchA;
  lastTouchB = touchB;
}

// ================= UPDATE =================
void updateMPR121() {

  uint16_t fA = rd16(0x04); // E0
  uint16_t fB = rd16(0x06); // E1

  // --- JOG A ---
  if (refA == 0) refA = fA;
  int diffA = refA - fA;

  if (diffA > 2) {
    if (cntA < 6) cntA++;
  } else {
    if (cntA > 0) cntA--;
  }
  touchA = (cntA >= 3);

  // --- JOG B ---
  if (refB == 0) refB = fB;
  int diffB = refB - fB;

  if (diffB > 1) {
    if (cntB < 4) cntB++;
  } else {
    if (cntB > 0) cntB--;
  }
  touchB = (cntB >= 3);

  sendTouchMidi();
}

// ================= API ===================
bool isJogTouchedA() { return touchA; }
bool isJogTouchedB() { return touchB; }
