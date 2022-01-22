#include <Arduino.h>
#include <ShiftRegister74HC595.h>

#include "Audio.h"
#include "Display.h"
#include "RGB_Strip.h"


RGB_Strip *rgb1, *rgb3;

[[noreturn]] void mainTask(void *pv) {
    /// RGB demo
//    rgb1 = new RGB_Strip(1);
//    rgb3 = new RGB_Strip(3);
//    rgb1->inverse = true;
//    rgb1->setEffect(RGB_THEATER_RAINBOW);
//    rgb3->setEffect(RGB_THEATER_RAINBOW);
//    while (true) {
//        delay(3000);
//        rgb1->setEffect(RGB_OFF);
//        rgb3->setEffect(RGB_OFF);
//        delay(3000);
//        rgb1->setEffect(RGB_THEATER_RAINBOW);
//        rgb3->setEffect(RGB_THEATER_RAINBOW);
//    }
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);

    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    /// Display demo
//    displayInit();
//    displayTaskRun();

    xTaskCreatePinnedToCore(mainTask, "mainTask", 4096, nullptr, 1, nullptr, 0);
}

__attribute__((unused)) void loop() {
}
