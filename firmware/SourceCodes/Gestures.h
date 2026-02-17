#pragma once

#include <Arduino.h>
#include "Config.h"

// ===== INICJALIZACJA =====
void initGestures();
void updateGestures();

// ===== WYSYŁANIE MIDI =====
void sendGesture(uint8_t sensor, GestureType gesture, uint8_t value = 127);
