#include "State.h"

// transport
bool playA = false;
bool cueA  = false;
bool playB = false;
bool cueB  = false;

// shift / cue
uint8_t shiftCount = 0;
bool cueMixMode = false;

// deck / gestures
Deck activeDeck = DECK_A;
GestureMode activeGestureMode = MODE_NONE;

void toggleCueMix() {
  cueMixMode = !cueMixMode;
}
