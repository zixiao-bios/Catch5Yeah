#include <Arduino.h>
#include <SPIFFS.h>
#include <freertos/FreeRTOS.h>

#include "Audio.h"
#include "Display.h"
#include "RGB_Strip.h"
#include "Network.h"
#include "Claw.h"

RGB_Strip *rgb1, *rgb2, *rgb3, *rgb4;

[[noreturn]] void displayTask(void *pv) {
    lvgl_mutex = xSemaphoreCreateMutex();
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    displayInit();
    show_screen("setting");
    xSemaphoreGive(lvgl_mutex);

    while (true) {
        delay(5);
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        lv_task_handler();
        xSemaphoreGive(lvgl_mutex);
    }
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);
    Serial.println("\n===============start===============");


    // set pin
    pinMode(PIN_K1, INPUT);
    pinMode(PIN_K2, INPUT);
    pinMode(PIN_K3, INPUT);
    pinMode(PIN_K4, INPUT);
    pinMode(PIN_BTN, INPUT);


    // init WiFi
//    wifi_on();


    // init RGB
    rgb1 = new RGB_Strip(1);
    rgb1->setEffect(RGB_OFF);

    rgb2 = new RGB_Strip(2);
    rgb2->setEffect(RGB_OFF);

    rgb3 = new RGB_Strip(3);
    rgb3->setEffect(RGB_OFF);

    rgb4 = new RGB_Strip(4);
    rgb4->setEffect(RGB_OFF);


    // start SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed!");
    }


    xTaskCreatePinnedToCore(displayTask, "DisplayTask", 10000, nullptr, 1, nullptr, 1);
}

__attribute__((unused)) void loop() {}
