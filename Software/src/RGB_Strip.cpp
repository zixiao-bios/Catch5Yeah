#include "RGB_Strip.h"

RGB_Strip::RGB_Strip(int id) : id(id) {
    String("RGB" + String(id)).toCharArray(name, 5);
    switch (id) {
        case 1:
            dataPin = RGB1;
            ledNum = RGB1_NUM_LEDS;
            break;
        case 2:
            dataPin = RGB2;
            ledNum = RGB2_NUM_LEDS;
            break;
        case 3:
            dataPin = RGB3;
            ledNum = RGB3_NUM_LEDS;
            break;
        case 4:
            dataPin = RGB4;
            ledNum = RGB4_NUM_LEDS;
            break;
        default:
            Serial.println("Error! Please set RGB id:" + String(id) + "in /src/RGB.cpp init function!");
    }
    leds = new CRGB[ledNum];

    switch (dataPin) {
        case 1:
            CFastLED::addLeds<NEOPIXEL, 1>(leds, ledNum);
            break;
        case 23:
            CFastLED::addLeds<NEOPIXEL, 23>(leds, ledNum);
            break;
        case 25:
            CFastLED::addLeds<NEOPIXEL, 25>(leds, ledNum);
            break;
        case 27:
            CFastLED::addLeds<NEOPIXEL, 27>(leds, ledNum);
            break;
        default:
            Serial.println("Error! Please set dataPin:" + String(dataPin) + "in /src/RGB.cpp init function!");
    }
}

void RGB_Strip::setPixel(CRGB *Pixel, byte red, byte green, byte blue) {
    Pixel->r = red;
    Pixel->g = green;
    Pixel->b = blue;
}

void RGB_Strip::setAll(byte red, byte green, byte blue, CRGB *leds, int ledNum) {
    for (int i = 0; i < ledNum; i++) {
        setPixel(&leds[i], red, green, blue);
    }
    FastLED.show();
}

void RGB_Strip::Wheel(byte WheelPos, byte c[3]) {
    if (WheelPos < 85) {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    } else {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }
}

[[noreturn]] void RGB_Strip::rainbow(void *pv) {
    auto *self = (RGB_Strip *) pv;
    while (true) {
        byte c[3];
        uint16_t i, j;
        const TickType_t delayTick = 10 / portTICK_PERIOD_MS;

        for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
            for (i = 0; i < self->ledNum; i++) {
                Wheel(((i * 256 / self->ledNum) + j) & 255, c);
                setPixel(&self->leds[i], *c, *(c + 1), *(c + 2));
            }
            FastLED.show();
            vTaskDelay(delayTick);
        }
    }
}

bool RGB_Strip::setEffect(const String &effect) {
    if (this->taskHandle != nullptr) {
        vTaskDelete(this->taskHandle);
        this->taskHandle = nullptr;
    }

//    setPixel(&leds[0], 255, 255, 255);
//    FastLED.show();
//    vTaskDelay(500 / portTICK_PERIOD_MS);
//    setPixel(&leds[0], 0, 0, 0);
//    FastLED.show();

    if (effect == "rainbow") {
        return xTaskCreate(&RGB_Strip::rainbow, this->name, 2048, this, 1, &this->taskHandle) == pdPASS;
    }
}
