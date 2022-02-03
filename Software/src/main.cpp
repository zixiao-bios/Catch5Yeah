#include <Arduino.h>
#include <ShiftRegister74HC595.h>

#include "Audio.h"
#include "Display.h"
#include "RGB_Strip.h"
#include "Network.h"
#include <Tool.h>
#include "SPIFFS.h"

RGB_Strip *rgb1, *rgb3;

[[noreturn]] void mainTask(void *pv) {
    /// Display demo
    displayInit();
    mainScreenLoad();

    while (true) {
        delay(100);
    }

    /// Network demo
//    WiFiInit();
//    printWifiList();
//    WiFiConnect();
//
//    while (true) {
//        if (WiFiClass::status() != WL_CONNECTED) {
//            WiFiConnect();
//        }
//
//        httpGet("https://www.runoob.com/http/http-tutorial.html");
//        delay(5000);
//    }

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

    pinMode(K1, INPUT);
    pinMode(K2, INPUT);
    pinMode(K3, INPUT);
    pinMode(K4, INPUT);
    pinMode(BTN, INPUT);

    // start SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed!");
    }

    xTaskCreatePinnedToCore(mainTask, "mainTask", 50000, nullptr, 1, nullptr, 0);
}

__attribute__((unused)) void loop() {
}
