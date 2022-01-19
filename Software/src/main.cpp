#include <Arduino.h>
#include <ShiftRegister74HC595.h>

#include "Audio.h"
#include "Display.h"
#include "RGB_Strip.h"


RGB_Strip *rgb1;
RGB_Strip *rgb3;

[[noreturn]] void mainTask(void *pv) {
    rgb3 = new RGB_Strip(3);
    rgb1 = new RGB_Strip(1);
    rgb1->inverse = true;
    rgb3->inverse = true;
    rgb1->setEffect("theaterRainbow");
    rgb3->setEffect("rainbow");

    delay(3000);
    rgb1->turnOff();
    rgb3->turnOff();
    while (true) {
        delay(1000);
//        rgb3->setEffect("loopRGB");
//        rgb2->setEffect("loopRGB");
    }
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);

//    displayInit();
//    displayTaskRun();

    // todo: 这里优先级=1时卡死，=2时正常，为什么？
    xTaskCreatePinnedToCore(mainTask, "mainTask", 10000, nullptr, 2, nullptr, 0);
}

__attribute__((unused)) void loop() {
}
