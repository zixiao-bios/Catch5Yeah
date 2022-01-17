#include <Arduino.h>
#include <ShiftRegister74HC595.h>

#include "Audio.h"
#include "Display.h"
#include "RGB_Strip.h"


__attribute__((unused)) void setup() {
    Serial.begin(115200);

//    displayInit();
//    displayTaskRun();

    static RGB_Strip rgb3 = RGB_Strip(3);
    rgb3.setEffect("rainbow");
}

__attribute__((unused)) void loop() {
    vTaskDelay(1);
}