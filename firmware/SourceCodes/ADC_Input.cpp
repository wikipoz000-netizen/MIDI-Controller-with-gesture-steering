#include "ADC_Input.h"
#include "MIDI.h"
#include "State.h"
#include "Config.h"
#include <Arduino.h>

/* =====================================================
   ================= KONFIGURACJA ======================
   ===================================================== */

// --- piny MUX ---
#define MUX_S0 5   // LSB
#define MUX_S1 4
#define MUX_S2 3
#define MUX_S3 2   // MSB

#define ADC_PIN   A1
#define XFADER_PIN A2

// --- filtry ---
#define RAW_DEADZONE        15     // minimalna zmiana ADC
#define MIDI_DEADZONE       3     // minimalna zmiana 0–127

/* =====================================================
   ================= MAPOWANIE CC ======================
   ===================================================== */

// 0–5   Deck A
// 6–11  Deck B
// 12–15 Center
const uint8_t potCC[16] = {
  // Deck A
  10, 11, 12, 13, 14, 15,

  // Deck B
  20, 21, 22, 23, 24, 25,

  // Center
  26, 27, 28, 29
};


// --- alternatywny CC dla CUE MIX ---
#define POT_CUE_MIX_INDEX     1     // fizyczny potencjometr
#define POT_CUE_MIX_ALT_CC   40     // CC w trybie cueMixMode

/* =====================================================
   ================= ZMIENNE ===========================
   ===================================================== */

uint8_t potValues[16];        // do debug / UI
uint8_t crossfaderValue;

static int lastRaw[16];
static uint8_t lastSent[16];
static bool adcInitialized = false;

/* =====================================================
   ================= MUX ===============================
   ===================================================== */

void selectMux(uint8_t ch) {
  digitalWrite(MUX_S0, (ch >> 0) & 1);
  digitalWrite(MUX_S1, (ch >> 1) & 1);
  digitalWrite(MUX_S2, (ch >> 2) & 1);
  digitalWrite(MUX_S3, (ch >> 3) & 1);
}

/* =====================================================
   ================= INIT ==============================
   ===================================================== */

void initADC() {

  analogReadResolution(10);

  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);

  for (int i = 0; i < 16; i++) {
    lastRaw[i]  = -1;
    lastSent[i] = 255;
    potValues[i] = 0;
  }

  crossfaderValue = 0;
  adcInitialized = true;
}

/* =====================================================
   ================= UPDATE ============================
   ===================================================== */

void updateADC() {

  if (!adcInitialized) return;

  /* ---------- POTENCJOMETRY ---------- */
  for (uint8_t i = 0; i < 16; i++) {

    // wybór kanału MUX
    selectMux(i);
    delayMicroseconds(6);

    // dummy reads (stabilizacja ADC + MUX)
    analogRead(ADC_PIN);
    analogRead(ADC_PIN);

    int raw = analogRead(ADC_PIN);

    // filtr RAW
    if (lastRaw[i] >= 0 && abs(raw - lastRaw[i]) < RAW_DEADZONE)
      continue;

    lastRaw[i] = raw;

    // mapowanie
    uint8_t val = map(raw, 0, 1023, 0, 127);
    potValues[i] = val;

    // filtr MIDI
    if (abs((int)val - (int)lastSent[i]) < MIDI_DEADZONE)
      continue;

    lastSent[i] = val;


    // ===== WYBÓR CC I KANAŁU =====
    uint8_t cc = potCC[i];
    uint8_t ch;

    if (i <= 5) {
      ch = MIDI_CH_A;
    }
    else if (i <= 11) {
      ch = MIDI_CH_B;
    }
    else {
      ch = 1; // center = global
    }

    // alternatywny tryb CUE MIX
    if (i == POT_CUE_MIX_INDEX && cueMixMode) {
      cc = POT_CUE_MIX_ALT_CC;
    }

    sendCC(ch, cc, val);

  }

  /* ---------- CROSSFADER ---------- */

  analogRead(XFADER_PIN);
  analogRead(XFADER_PIN);

  int rawXF = analogRead(XFADER_PIN);
  uint8_t xf = map(rawXF, 0, 1023, 0, 127);

  crossfaderValue = xf;

  static uint8_t lastXF = 255;
  if (abs((int)xf - (int)lastXF) >= MIDI_DEADZONE) {
    lastXF = xf;
    sendCC(1, 8, xf);   // global crossfader
  }
}
