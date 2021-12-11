#include <Arduino.h>
#include <SPIFFS.h>

#include "Audio.h"

// unused
#include <TFT_eSPI.h>

Audio *audio;

__attribute__((unused)) void setup() {
    Serial.begin(115200);
    Serial.println("=============start==============\n");

    SPIFFS.begin();
    audio = new Audio();
    audio->play("/yeshanghai_duan.mp3");
    audio->setPlayMode(Loop);
}

__attribute__((unused)) void loop() {
}