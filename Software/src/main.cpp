#include <Arduino.h>
#include <ShiftRegister74HC595.h>

#include "Audio.h"
#include "Display.h"
#include "RGB_Strip.h"
#include "../MyConfig.h"

#include <WiFi.h>

const String url = "https://www.runoob.com/http/http-tutorial.html";
RGB_Strip *rgb1, *rgb3;

void printWifiList() {
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
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            delay(10);
        }
    }
    Serial.println("=======================================\n\n");
}

void wifiConnect() {
    Serial.print("WiFi connecting");
    WiFi.begin(SSID, PASSWORD);
    while (WiFiClass::status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

[[noreturn]] void mainTask(void *pv) {
    WiFiClass::mode(WIFI_STA);
    wifiConnect();

    while (true) {
        if (WiFiClass::status() != WL_CONNECTED) {
            wifiConnect();
        }

        HTTPClient http;

        Serial.println("[HTTP] begin...");
        http.begin(url);

        Serial.println("[HTTP] GET...");
        int httpCode = http.GET();

        if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        delay(5000);
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

    xTaskCreatePinnedToCore(mainTask, "mainTask", 10000, nullptr, 1, nullptr, 0);
}

__attribute__((unused)) void loop() {
}
