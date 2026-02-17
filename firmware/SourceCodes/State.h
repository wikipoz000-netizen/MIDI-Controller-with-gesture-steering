#pragma once
#include <Arduino.h>
#include "Config.h"

// SHIFT
extern uint8_t shiftCount;

// tryby / deck
extern bool cueMixMode;
extern Deck activeDeck;

// ⬇⬇⬇ JEDYNY TRYB GESTÓW ⬇⬇⬇
extern GestureMode activeGestureMode;

// GPIO
extern bool playA;
extern bool cueA;
extern bool playB;
extern bool cueB;

void toggleCueMix();
