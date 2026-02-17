#pragma once
#include <Arduino.h>

void initGPIO();
void updateGPIO();

// debug (globalne odczyty)
extern volatile int32_t jogA_pos;
extern volatile int32_t jogB_pos;
extern volatile int32_t browse_pos;

extern int8_t jogStepA;
extern int8_t jogStepB;
extern int8_t browseStep;
