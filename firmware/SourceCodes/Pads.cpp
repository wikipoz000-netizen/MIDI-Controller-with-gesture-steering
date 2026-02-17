#include "Pads.h"
#include "Config.h"
#include "State.h"
#include "MIDI.h"

static int lastPad = -1;
static uint32_t lastPadTime = 0;
#define PAD_DEBOUNCE_MS 200

bool near(int v, int t) {
  return abs(v - t) < PAD_TOL;
}

void initPads() {
  analogReadResolution(10);
}

void updatePads() {

  int v = analogRead(A0);
  int pad = -1;

  if      (near(v, PAD_A1)) pad = 0;
  else if (near(v, PAD_A2)) pad = 1;
  else if (near(v, PAD_A3)) pad = 2;
  else if (near(v, PAD_A4)) pad = 3;
  else if (near(v, PAD_B1)) pad = 4;
  else if (near(v, PAD_B2)) pad = 5;
  else if (near(v, PAD_B3)) pad = 6;
  else if (near(v, PAD_B4)) pad = 7;

  if (pad == -1) {
    lastPad = -1;
    return;
  }

  if (pad == lastPad && millis() - lastPadTime < PAD_DEBOUNCE_MS)
    return;

  lastPad = pad;
  lastPadTime = millis();

  bool shift = (shiftCount > 0);

  /* =====================================================
     =============== BEZ SHIFT ===========================
     ===================================================== */
  if (!shift) {

    // ===== PERFORMANCE PADS =====
    if (pad <= 3) {
      // Deck A
      activeDeck = DECK_A;
      sendNote(MIDI_CH_A, 32 + pad, 127);   // 32–35
    }
    else {
      // Deck B
      activeDeck = DECK_B;
      sendNote(MIDI_CH_B, 64 + (pad - 4), 127); // 64–67
    }

    return;
  }

  /* =====================================================
     =============== SHIFT = TRYBY GESTÓW ================
     ===================================================== */

  // wybór decka
  if (pad <= 3) {
    activeDeck = DECK_A;
  }
  else {
    activeDeck = DECK_B;
  }

  // wybór trybu gestów
  switch (pad) {

    // Deck A
    case 0: activeGestureMode = MODE_EQ;        break;
    case 1: activeGestureMode = MODE_FX;        break;
    case 2: activeGestureMode = MODE_DYNAMICS;  break;
    case 3: activeGestureMode = MODE_MACRO;     break;

    // Deck B
    case 4: activeGestureMode = MODE_EQ;        break;
    case 5: activeGestureMode = MODE_FX;        break;
    case 6: activeGestureMode = MODE_DYNAMICS;  break;
    case 7: activeGestureMode = MODE_MACRO;     break;
  }
}

