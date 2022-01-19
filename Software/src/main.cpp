#include <Arduino.h>
#include <ShiftRegister74HC595.h>

#include "Audio.h"
#include "Display.h"
#include "RGB_Strip.h"


RGB_Strip *rgb2;
RGB_Strip *rgb3;

[[noreturn]] void mainTask(void *pv) {
    while (true) {
        delay(1000);
        rgb3->setEffect("theaterRainbow");
//        rgb2->turnOff();
        delay(1000);
//        rgb2->setEffect("rainbow");
        rgb3->turnOff();
    }
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);

//    displayInit();
//    displayTaskRun();

    rgb3 = new RGB_Strip(3);
    rgb2 = new RGB_Strip(2);
    rgb2->inverse = true;
    rgb3->inverse = true;
    rgb2->setEffect("theaterRainbow");
    rgb3->setEffect("rainbow");

    xTaskCreatePinnedToCore(mainTask, "mainTask", 10000, nullptr, 2, nullptr, 0);
}

__attribute__((unused)) void loop() {
}
