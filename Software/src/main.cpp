#include <Arduino.h>
#include <ShiftRegister74HC595.h>

#include "Audio.h"
#include "Display.h"
#include "RGB_Strip.h"


RGB_Strip *rgb2;
RGB_Strip *rgb3;

[[noreturn]] void mainTask(void *pv) {
    rgb3 = new RGB_Strip(3);
    rgb2 = new RGB_Strip(2);
    rgb2->inverse = true;
    rgb3->inverse = true;
    rgb2->setEffect("loopRGB");
    rgb3->setEffect("rainbow");

    while (true) {
        delay(1000);
        rgb3->setEffect("loopRGB");
        rgb2->turnOff();
        delay(1000);
        rgb2->setEffect("rainbow");
        rgb3->turnOff();
    }
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);

//    displayInit();
//    displayTaskRun();

    xTaskCreatePinnedToCore(mainTask, "mainTask", 10000, nullptr, 10, nullptr, 0);
}

__attribute__((unused)) void loop() {
}
