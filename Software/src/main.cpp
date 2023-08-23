#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <LITTLEFS.h>

#include "Display.h"
#include "RGB_Strip.h"
#include "Network.h"
#include "Claw.h"
#include "Setup.h"
#include "RTC.h"
#include "Tool.h"
#include "music.h"

RGB_Strip *rgb1, *rgb2, *rgb3, *rgb4;

[[noreturn]] void displayTask(void *pv) {
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

    // start LittleFS
    if (!LITTLEFS.begin(true)) {
        Serial.println("LittleFS Mount Failed!");
    }

    rtc_init();

    // read setup file
    init_setup();

    // init music player
    music_init();

    // init claw
    claw_init();
    claw_reset_async();

    // init display
    displayInit();
    show_screen("main");
    xTaskCreatePinnedToCore(displayTask, "DisplayTask", 10000, nullptr, 1, nullptr, 1);

    // init WiFi
    if (setup_doc["wifi"]["enable"]) {
        xTaskCreatePinnedToCore(UI_turn_on_wifi_task, "TurnOnWifi", 10000, nullptr, 1, nullptr, 0);
    }

    // init RGB
    rgb1 = new RGB_Strip(1);
    rgb1->setEffect(RGB_RAINBOW);

//    rgb2 = new RGB_Strip(2);
//    rgb2->setEffect(RGB_OFF);

    rgb3 = new RGB_Strip(3);
    rgb3->setEffect(RGB_RAINBOW);

    rgb4 = new RGB_Strip(4);
    rgb4->setEffect(RGB_RAINBOW);
}

__attribute__((unused)) void loop() {}
