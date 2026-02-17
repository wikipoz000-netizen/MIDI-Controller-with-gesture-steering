#pragma once
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

void initMIDI();

void sendCC(uint8_t ch, uint8_t cc, uint8_t val);
void sendNote(uint8_t ch, uint8_t note, uint8_t vel);
void sendPitchBend(uint8_t ch, int value);
void sendNudge(uint8_t ch, int delta);
void sendScratch(uint8_t ch, int delta);

void sendCCFiltered(uint8_t ch, uint8_t cc, uint8_t val);
