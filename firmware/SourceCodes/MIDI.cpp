#include "MIDI.h"
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

/* =====================================================
   ================= USB MIDI ==========================
   ===================================================== */

Adafruit_USBD_MIDI usb_midi;

/* =====================================================
   ================= PARAMETRY =========================
   ===================================================== */

#define MIDI_CC_DEADZONE  0   // globalny próg CC (3–5 zalecane)

/* =====================================================
   ================= STAN FILTRA =======================
   ===================================================== */

// [kanał 0–15][cc 0–127]
static int lastCCValue[16][128];

/* =====================================================
   ================= RAW SEND ==========================
   ===================================================== */

static void sendPacket(uint8_t cin, uint8_t b0, uint8_t b1, uint8_t b2) {
  uint8_t packet[4] = { cin, b0, b1, b2 };
  usb_midi.write(packet, 4);
}

/* =====================================================
   ================= INIT ==============================
   ===================================================== */

void initMIDI() {

  usb_midi.begin();

  for (int ch = 0; ch < 16; ch++) {
    for (int cc = 0; cc < 128; cc++) {
      lastCCValue[ch][cc] = -1;
    }
  }
}

/* =====================================================
   ================= HIGH LEVEL MIDI ===================
   ===================================================== */

// ===== CONTROL CHANGE (FILTROWANY GLOBALNIE) =====
void sendCC(uint8_t ch, uint8_t cc, uint8_t val) {
  if (ch < 1 || ch > 16) return;

  uint8_t chIdx = ch - 1;
  val &= 0x7F;

  int last = lastCCValue[chIdx][cc];
  if (last >= 0 && abs(val - last) < MIDI_CC_DEADZONE) return;

  lastCCValue[chIdx][cc] = val;

  sendPacket(
    0x0B,                        // CIN = Control Change
    0xB0 | (chIdx & 0x0F),
    cc & 0x7F,
    val & 0x7F
  );
}




// ===== NOTE ON / OFF =====
void sendNote(uint8_t ch, uint8_t note, uint8_t vel) {

  if (ch < 1 || ch > 16) return;

uint8_t cin    = (vel > 0) ? 0x09 : 0x08;
uint8_t status = (vel > 0) ? 0x90 : 0x80;

sendPacket(
  cin,
  status | ((ch - 1) & 0x0F),
  note & 0x7F,
  vel & 0x7F
);

}

// ===== PITCH BEND =====
void sendPitchBend(uint8_t ch, int value) {

  if (ch < 1 || ch > 16) return;

  value = constrain(value, -8192, 8191);
  uint16_t v = value + 8192;

  sendPacket(
    0x0E,                         // CIN: Pitch Bend
    0xE0 | ((ch - 1) & 0x0F),
    v & 0x7F,
    (v >> 7) & 0x7F
  );
}

// ===== TEMPO NUDGE (JOG) =====
void sendNudge(uint8_t ch, int delta) {
  if (delta == 0) return;        // <<< KLUCZ

  delta = constrain(delta, -1, 1);

  uint8_t val = (delta > 0) ? 65 : 63;
  sendCC(ch, 64, val);
}


// ===== SCRATCH (PITCH BEND) =====
void sendScratch(uint8_t ch, int delta) {
  sendPitchBend(ch, delta * 256);
}
