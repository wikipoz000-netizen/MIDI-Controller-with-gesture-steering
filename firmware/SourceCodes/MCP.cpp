#include "MCP.h"
#include <Adafruit_MCP23X17.h>
#include "State.h"
#include "MIDI.h"

/* =====================================================
   ================= MCP INSTANCE ======================
   ===================================================== */

Adafruit_MCP23X17 mcp;

/* =====================================================
   ================= PIN MAP ===========================
   ===================================================== */

// Deck A
#define MCP_SHIFT_A     11
#define MCP_LOOP_A      12
#define MCP_ENC_A_A     13
#define MCP_ENC_A_B     14

// Deck B
#define MCP_SHIFT_B     3
#define MCP_LOOP_B      4
#define MCP_ENC_B_A     1
#define MCP_ENC_B_B     2

// Center
#define MCP_LOAD_A      9
#define MCP_LOAD_B      6
#define MCP_CUE_A       8
#define MCP_CUE_B       7
#define MCP_CUE_MODE    10
#define MCP_AUTOMIX     5

/* =====================================================
   ================= DEBOUNCE ==========================
   ===================================================== */

#define DEBOUNCE_MS 30

/* =====================================================
   ================= BUTTON STRUCT =====================
   ===================================================== */

struct Button {
  uint8_t pin;
  bool state;
  bool last;
  uint32_t lastTime;
};

static Button mcpButtons[] = {
  { MCP_SHIFT_A, false, false, 0 },
  { MCP_LOOP_A,  false, false, 0 },
  { MCP_SHIFT_B, false, false, 0 },
  { MCP_LOOP_B,  false, false, 0 },
  { MCP_LOAD_A,  false, false, 0 },
  { MCP_LOAD_B,  false, false, 0 },
  { MCP_CUE_A,   false, false, 0 },
  { MCP_CUE_B,   false, false, 0 },
  { MCP_CUE_MODE,false, false, 0 },
  { MCP_AUTOMIX, false, false, 0 }
};

constexpr uint8_t BTN_COUNT = sizeof(mcpButtons) / sizeof(Button);

/* =====================================================
   ================= ENCODER STRUCT ====================
   ===================================================== */

struct Encoder {
  uint8_t pinA;
  uint8_t pinB;
  uint8_t last;
  uint8_t notePlus;
  uint8_t noteMinus;
};

// LOOP encoders (NOTE)
static Encoder loopEncA = { MCP_ENC_A_A, MCP_ENC_A_B, 0, 78, 79 };
static Encoder loopEncB = { MCP_ENC_B_A, MCP_ENC_B_B, 0, 72, 73 };

/* =====================================================
   ================= INIT ==============================
   ===================================================== */

void initMCP() {

  if (!mcp.begin_I2C(0x20)) {
    while (1); // MCP not found
  }

  for (uint8_t i = 0; i < 16; i++) {
    mcp.pinMode(i, INPUT_PULLUP);
  }

  loopEncA.last =
    (mcp.digitalRead(loopEncA.pinA) << 1) |
     mcp.digitalRead(loopEncA.pinB);

  loopEncB.last =
    (mcp.digitalRead(loopEncB.pinA) << 1) |
     mcp.digitalRead(loopEncB.pinB);
}

/* =====================================================
   ================= BUTTON PRESS ======================
   ===================================================== */

void handleButtonPress(uint8_t pin) {

  // -------- SHIFT --------
  if (pin == MCP_SHIFT_A || pin == MCP_SHIFT_B) {
    shiftCount++;
    return;
  }

  // -------- LOOP / SYNC --------
  if (pin == MCP_LOOP_A) {
    if (shiftCount > 0) sendNote(MIDI_CH_A, 60, 127); // SYNC
    else               sendNote(MIDI_CH_A, 61, 127); // LOOP
    return;
  }

  if (pin == MCP_LOOP_B) {
    if (shiftCount > 0) sendNote(MIDI_CH_B, 62, 127);
    else               sendNote(MIDI_CH_B, 63, 127);
    return;
  }

  // -------- LOAD --------
  if (pin == MCP_LOAD_A) sendNote(MIDI_CH_A, 20, 127);
  if (pin == MCP_LOAD_B) sendNote(MIDI_CH_B, 21, 127);

  // -------- CUE --------
  if (pin == MCP_CUE_A) sendNote(MIDI_CH_A, 22, 127);
  if (pin == MCP_CUE_B) sendNote(MIDI_CH_B, 23, 127);

  // -------- CUE MIX TOGGLE (LOGIC ONLY) --------
  if (pin == MCP_CUE_MODE) {
    toggleCueMix();
  }

  // -------- AUTOMIX --------
  if (pin == MCP_AUTOMIX) {
    sendNote(1, 91, 127);
  }
}

/* =====================================================
   ================= BUTTON RELEASE ====================
   ===================================================== */

void handleButtonRelease(uint8_t pin) {
  if (pin == MCP_SHIFT_A || pin == MCP_SHIFT_B) {
    if (shiftCount > 0) shiftCount--;
  }
}

/* =====================================================
   ================= LOOP ENCODER ======================
   ===================================================== */

void updateLoopEncoder(Encoder &e, uint8_t ch) {

  uint8_t state =
    (mcp.digitalRead(e.pinA) << 1) |
     mcp.digitalRead(e.pinB);

  if (state == 0b00 && e.last != 0b00) {

    if (e.last == 0b10) {
      sendNote(ch, e.notePlus, 127);
    }
    else if (e.last == 0b01) {
      sendNote(ch, e.noteMinus, 127);
    }
  }

  e.last = state;
}

/* =====================================================
   ================= UPDATE ============================
   ===================================================== */

void updateMCP() {

  // -------- BUTTONS --------
  for (uint8_t i = 0; i < BTN_COUNT; i++) {

    bool r = !mcp.digitalRead(mcpButtons[i].pin);

    if (r != mcpButtons[i].last) {
      mcpButtons[i].lastTime = millis();
    }

    if (millis() - mcpButtons[i].lastTime > DEBOUNCE_MS) {
      if (r != mcpButtons[i].state) {
        mcpButtons[i].state = r;

        if (r) handleButtonPress(mcpButtons[i].pin);
        else   handleButtonRelease(mcpButtons[i].pin);
      }
    }

    mcpButtons[i].last = r;
  }

  // -------- LOOP ENCODERS --------
  updateLoopEncoder(loopEncA, MIDI_CH_A);
  updateLoopEncoder(loopEncB, MIDI_CH_B);
}
