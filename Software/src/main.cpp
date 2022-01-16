#include <Arduino.h>
#include <SPIFFS.h>
#include <FastLED.h>
#include "Audio.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include "FS.h"
#include <ShiftRegister74HC595.h>
#include "Display.h"


__attribute__((unused)) void setup() {
    Serial.begin(115200);

    displayInit();
    displayTaskRun();
}

__attribute__((unused)) void loop() {
}