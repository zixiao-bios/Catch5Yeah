#include <Arduino.h>
#include <ShiftRegister74HC595.h>

#include "Audio.h"
#include "Display.h"
#include "RGB_Strip.h"

#include <WiFi.h>


RGB_Strip *rgb1, *rgb3;

[[noreturn]] void mainTask(void *pv) {
    WiFiClass::mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    Serial.println("setup done");

    while (true) {
        Serial.println("scan start");
        int n = WiFi.scanNetworks();
        Serial.println("snan done");

        if (n == 0) {
            Serial.println("no WiFi found");
        } else {
            Serial.print(n);
            Serial.println(" networks found");

            for (int i = 0; i < n; ++i) {
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(WiFi.SSID(i));
                Serial.print(" (");
                Serial.print(WiFi.RSSI(i));
                Serial.print(")");
                Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
                delay(10);
            }
        }
        Serial.println("=======================================\n\n");
        delay(1000);
    }

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

    /// Display demo
//    displayInit();
//    displayTaskRun();

    xTaskCreatePinnedToCore(mainTask, "mainTask", 4096, nullptr, 1, nullptr, 0);
}

__attribute__((unused)) void loop() {
}
