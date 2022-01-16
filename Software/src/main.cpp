#include <Arduino.h>
#include <FastLED.h>
#include <ShiftRegister74HC595.h>

#include "Audio.h"
#include "Display.h"


__attribute__((unused)) void setup() {
    Serial.begin(115200);

    displayInit();
    displayTaskRun();
}

__attribute__((unused)) void loop() {
}