#include "GPIO_Input.h"
#include "Config.h"
#include "MIDI.h"
#include "MPR121_Touch.h"
#include <Arduino.h>

/* =====================================================
   ================= ENCODER GRAY TABLE ================
   ===================================================== */

static const int8_t ENC_TABLE[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

/* =====================================================
   ================= ENCODER STRUCT ====================
   ===================================================== */

struct Encoder {
  uint8_t pinA;
  uint8_t pinB;
  uint8_t lastState;
  int8_t  step;      // akumulacja kroków
};

/* =====================================================
   ================= ENCODERS ==========================
   ===================================================== */

static Encoder jogA = {
  JOG_A_A,
  JOG_A_B,
  0,
  0
};

static Encoder jogB = {
  JOG_B_A,
  JOG_B_B,
  0,
  0
};

static Encoder browse = {
  BROWSE_A,
  BROWSE_B,
  0,
  0
};
// ===== DEBUG MIRRORS =====
int8_t jogStepA = 0;
int8_t jogStepB = 0;
int8_t browseStep = 0;
static int8_t  browseAccum = 0;
static uint32_t browseLastMove = 0;

/* =====================================================
   ================= BUTTON STRUCT =====================
   ===================================================== */

struct Button {
  uint8_t pin;
  bool state;
  bool last;
  uint32_t lastTime;
};

static Button buttons[] = {
  { PLAY_A_PIN, false, false, 0 },
  { CUE_A_PIN,  false, false, 0 },
  { PLAY_B_PIN, false, false, 0 },
  { CUE_B_PIN,  false, false, 0 }
};

constexpr uint8_t BTN_COUNT = sizeof(buttons) / sizeof(Button);

/* =====================================================
   ================= INIT ==============================
   ===================================================== */

void initGPIO() {

  // --- ENCODERS ---
  pinMode(JOG_A_A, INPUT_PULLUP);
  pinMode(JOG_A_B, INPUT_PULLUP);
  pinMode(JOG_B_A, INPUT_PULLUP);
  pinMode(JOG_B_B, INPUT_PULLUP);
  pinMode(BROWSE_A, INPUT_PULLUP);
  pinMode(BROWSE_B, INPUT_PULLUP);

  jogA.lastState =
    (digitalRead(jogA.pinA) << 1) | digitalRead(jogA.pinB);
  jogB.lastState =
    (digitalRead(jogB.pinA) << 1) | digitalRead(jogB.pinB);
  browse.lastState =
    (digitalRead(browse.pinA) << 1) | digitalRead(browse.pinB);

  // --- BUTTONS ---
  pinMode(PLAY_A_PIN, INPUT_PULLUP);
  pinMode(CUE_A_PIN,  INPUT_PULLUP);
  pinMode(PLAY_B_PIN, INPUT_PULLUP);
  pinMode(CUE_B_PIN,  INPUT_PULLUP);
}

/* =====================================================
   ================= ENCODER READ ======================
   ===================================================== */

static inline int8_t readEncoder(Encoder &e) {

  uint8_t s =
    (digitalRead(e.pinA) << 1) |
     digitalRead(e.pinB);

  uint8_t idx = (e.lastState << 2) | s;
  int8_t delta = ENC_TABLE[idx];

  e.lastState = s;
  return delta;
}

/* =====================================================
   ================= SEND RELATIVE CC ==================
   ===================================================== */

static inline void sendRelativeCC(uint8_t ch, uint8_t cc, int8_t step) {
  if (step == 0) return;

  uint8_t val = (step > 0) ? 65 : 63;
  sendCC(ch, cc, val);
}

/* =====================================================
   ================= UPDATE ============================
   ===================================================== */

void updateGPIO() {

  /* ---------- ENCODERS ---------- */

  jogA.step   += readEncoder(jogA);
  jogB.step   += readEncoder(jogB);
  browse.step += readEncoder(browse);

// --- JOG A ---
if (jogA.step != 0) {
  if (isJogTouchedA())
    sendPitchBend(MIDI_CH_A, jogA.step * 256);
  else
    sendRelativeCC(MIDI_CH_A, 42, jogA.step);

  jogStepA = jogA.step;
  jogA.step = 0;
}

// --- JOG B ---
if (jogB.step != 0) {
  if (isJogTouchedB())
    sendPitchBend(MIDI_CH_B, jogB.step * 256);
  else
    sendRelativeCC(MIDI_CH_B, 74, jogB.step);

  jogStepB = jogB.step;
  jogB.step = 0;
}

// --- BROWSE (gesture-based) ---

// akumuluj kroki
if (browse.step != 0) {
  browseAccum += browse.step;
  browseLastMove = millis();
  browse.step = 0;
}

// jeśli enkoder się zatrzymał
if (browseAccum != 0 && millis() - browseLastMove > 20) {

  int8_t mag = abs(browseAccum);

  if (mag >= 1 && mag <= 6) {

    if (browseAccum > 0)
      sendNote(1, 80, 127);   // BROWSE DOWN
    else
      sendNote(1, 81, 127);   // BROWSE UP
  }

  // reset gestu
  browseAccum = 0;
}

  /* ---------- BUTTONS ---------- */

  for (uint8_t i = 0; i < BTN_COUNT; i++) {

    bool r = !digitalRead(buttons[i].pin);

    if (r != buttons[i].last)
      buttons[i].lastTime = millis();

    if (millis() - buttons[i].lastTime > 30) {
      if (r != buttons[i].state) {
        buttons[i].state = r;

        if (r) {
          if (buttons[i].pin == PLAY_A_PIN)
            sendNote(MIDI_CH_A, 36, 127);
          if (buttons[i].pin == CUE_A_PIN)
            sendNote(MIDI_CH_A, 37, 127);
          if (buttons[i].pin == PLAY_B_PIN)
            sendNote(MIDI_CH_B, 68, 127);
          if (buttons[i].pin == CUE_B_PIN)
            sendNote(MIDI_CH_B, 69, 127);
        }
      }
    }

    buttons[i].last = r;
  }  
}
