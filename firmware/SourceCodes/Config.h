#pragma once
#include <Arduino.h>

// ===== I2C (RP2040 / Pico) =====
#define SDA_PIN 0
#define SCL_PIN 1

// ===== MIDI CHANNELS =====
#define MIDI_CH_A 1
#define MIDI_CH_B 2

// ===== DECKI =====
enum Deck {
  DECK_A = 0,
  DECK_B = 1
};

// ===== TRYBY GESTÓW (JEDYNA DEFINICJA) =====
enum GestureMode {
  MODE_EQ = 0,
  MODE_FX,
  MODE_DYNAMICS,
  MODE_MACRO,
  MODE_NONE
};


// ===== TYPY GESTÓW =====
enum GestureType {
  G_UPDOWN = 0,   // ciągły CC (góra/dół)
  G_TAP,          // szybkie machnięcie nad jednym czujnikiem
  G_SWIPE_L,      // globalny swipe right -> left
  G_SWIPE_R,      // globalny swipe left -> right
  G_HAND_ON,      // ręka pojawia się nad czujnikiem
  G_HAND_OFF      // ręka znika
};


// ===== TYP MIDI =====
enum MidiType {
  MIDI_CC = 0,
  MIDI_NOTE
};

// ===== PERFORMANCE PADS (ADC) =====
#define PAD_TOL 40
#define PAD_A1 790
#define PAD_A2 680
#define PAD_A3 570
#define PAD_A4 450
#define PAD_B1 333
#define PAD_B2 212
#define PAD_B3 104
#define PAD_B4 2

// ===== GPIO ENCODERS =====
#define JOG_A_A     15
#define JOG_A_B     14

#define JOG_B_A     11
#define JOG_B_B     10

#define BROWSE_A    7
#define BROWSE_B    6

// ===== GPIO BUTTONS =====
#define PLAY_A_PIN  12
#define CUE_A_PIN   13

#define PLAY_B_PIN  9
#define CUE_B_PIN   8

