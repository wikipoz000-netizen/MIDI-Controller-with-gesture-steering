#include "Debug.h"
#include "State.h"
#include "Config.h"
#include <Arduino.h>

// ================= USTAWIENIA =================
#define DEBUG_ENABLED 1
#define DEBUG_INTERVAL_MS 100

static uint32_t lastPrint = 0;

// ====== ZMIENNE Z INNYCH MODUŁÓW ======

// ADC / Potencjometry
extern uint8_t potValues[16];
extern uint8_t crossfaderValue;

// Enkodery
extern int8_t jogStepA;
extern int8_t jogStepB;
extern int8_t browseStep;


// Przyciski (MCP)
extern bool playA, cueA, playB, cueB;
extern bool cueMixMode;

// Czujniki dystansu – debug
extern int dbgDist[3];    // CC 0–127, -1 brak
extern bool dbgActive[3]; // HAND ON

// Tryby
const char* modeName(GestureMode m) {
  switch (m) {
    case MODE_EQ:       return "EQ";
    case MODE_FX:       return "FX";
    case MODE_DYNAMICS: return "DY";
    case MODE_MACRO:    return "MC";
    default:            return "--";
  }
}

// ================= INIT =================
void initDebug() {
#if DEBUG_ENABLED
  Serial.begin(115200);
  while (!Serial) { delay(5); }
  Serial.println("=== DJ CONTROLLER DEBUG FULL ===");
#endif
}

// ================= UPDATE =================
void updateDebug() {
#if DEBUG_ENABLED
  uint32_t now = millis();
  if (now - lastPrint < DEBUG_INTERVAL_MS) return;
  lastPrint = now;

  // ===== GŁOWA =====
  Serial.print("D:");
  Serial.print(activeDeck == DECK_A ? "A" : "B");

  Serial.print(" GM:");
  Serial.print(modeName(activeGestureMode));

  Serial.print(" | SH:");
  Serial.print(shiftCount);

  Serial.print(" CM:");
  Serial.print(cueMixMode);

  // ===== POTENCJOMETRY =====
  Serial.print(" | P:[");
  for (int i = 0; i < 16; i++) {
    Serial.print(potValues[i]);
    if (i < 15) Serial.print(" ");
  }
  Serial.print("] XF:");
  Serial.print(crossfaderValue);

  // ===== ENKODERY =====
  Serial.print(" | E:[JA:");
  Serial.print(jogStepA);
  Serial.print(" JB:");
  Serial.print(jogStepB);
  Serial.print(" BR:");
  Serial.print(browseStep);
  Serial.print("]");

  // ===== PRZYCISKI =====
  Serial.print(" | B:[PA:");
  Serial.print(playA);
  Serial.print(" CA:");
  Serial.print(cueA);
  Serial.print(" PB:");
  Serial.print(playB);
  Serial.print(" CB:");
  Serial.print(cueB);
  Serial.print("]");

  // ===== DISTANCE SENSORS =====
Serial.print(" | DIST:[");

const char* name[3] = { "L", "C", "R" };

for (int i = 0; i < 3; i++) {
  Serial.print(name[i]);
  Serial.print(":");

  if (dbgDist[i] >= 0) {
    Serial.print(dbgDist[i]);
    if (dbgActive[i]) Serial.print("*");
  } else {
    Serial.print("--");
  }

  if (i < 2) Serial.print(" ");
}

Serial.print("]");

  Serial.println();
#endif
}
