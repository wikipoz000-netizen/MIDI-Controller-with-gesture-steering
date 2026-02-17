#include "Config.h"
#include "State.h"
#include "MIDI.h"
#include "MCP.h"
#include "GPIO_Input.h"
#include "ADC_Input.h"
#include "Pads.h"
#include "MPR121_Touch.h"
#include "Gestures.h"
#include "Debug.h"
#include <Wire.h>

void setup() {
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();

  initMIDI();
  initMCP();
  initGPIO();
  initADC();
  initPads();
  initMPR121();
  initGestures();
  initDebug();
}

void loop() {
  updateMCP();
  updateMPR121();
  updateGPIO();      // enkodery GPIO
  updateADC();
  updatePads();
  updateDebug();
  updateGestures();  // gesty NA KOŃCU
  delay(1);
}

