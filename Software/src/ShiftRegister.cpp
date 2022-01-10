#include <Arduino.h>
#include <SPIFFS.h>
#include <FastLED.h>

#include "Audio.h"

// unused
#include <TFT_eSPI.h>

#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>

#include <ShiftRegister74HC595.h>

ShiftRegister74HC595<1> sr(15, 0, 2); // NOLINT

void setup() {
    Serial.begin(115200);
    pinMode(35, INPUT);
}

void loop() {
    Serial.println(digitalRead(35));
    delay(50);
//    sr.setNoUpdate(1, HIGH);
//    sr.setNoUpdate(2, LOW);
//    sr.updateRegisters();
//    delay(2000);
//
//    sr.setNoUpdate(1, LOW);
//    sr.setNoUpdate(2, HIGH);
//    sr.updateRegisters();
//    delay(2000);
}