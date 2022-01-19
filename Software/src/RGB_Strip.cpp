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

    this->turnOff();
}

void RGB_Strip::setPixel(CRGB *Pixel, byte red, byte green, byte blue) {
    Pixel->r = red;
    Pixel->g = green;
    Pixel->b = blue;
}

void RGB_Strip::setAll(byte red, byte green, byte blue, CRGB *leds, int ledNum) {
    for (int i = ledNum - 1; i >= 0; i--) {
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

bool RGB_Strip::setEffect(const String &effect) {
    stopTask();

    if (effect == "rainbow") {
        return xTaskCreatePinnedToCore(&RGB_Strip::rainbow, this->name, 10000, this, 1, &this->taskHandle, 0) == pdPASS;
    } else if (effect == "theaterRainbow") {
        return xTaskCreatePinnedToCore(&RGB_Strip::theaterRainbow, this->name, 10000, this, 1, &this->taskHandle, 0) ==
               pdPASS;
    } else if (effect == "loopRGB") {
        // todo: 这里的优先级=2时卡死，=1时正常，为什么？
        return xTaskCreatePinnedToCore(&RGB_Strip::RGBLoop, this->name, 10000, this, 1, &this->taskHandle, 0) == pdPASS;
    } else if (effect == "test") {
        return xTaskCreatePinnedToCore(&RGB_Strip::test, this->name, 10000, this, 1, &this->taskHandle, 0) == pdPASS;
    }
}

[[noreturn]] void RGB_Strip::theaterRainbow(void *pv) {
    auto *self = (RGB_Strip *) pv;
    byte c[3];

    while (true) {
        for (int j = 0; j < 256; j++) {     // cycle all 256 colors in the wheel
            for (int q = 0; q < 3; q++) {
                for (int i = 0; i < self->ledNum; i = i + 3) {
                    Wheel((i + j) % 255, c);
                    if (self->inverse) {
                        if (i + q >= self->ledNum) {
                            break;
                        }
                        setPixel(&self->leds[i + q], *c, *(c + 1), *(c + 2));    //turn every third pixel on
                    } else {
                        if (self->ledNum - 1 - (i + q) < 0) {
                            break;
                        }
                        setPixel(&self->leds[self->ledNum - 1 - (i + q)], *c, *(c + 1),
                                 *(c + 2));    //turn every third pixel on
                    }
                }
                FastLED.show();
                delay(20);

                for (int i = 0; i < self->ledNum; i = i + 3) {
                    if (self->inverse) {
                        if (i + q >= self->ledNum) {
                            break;
                        }
                        setPixel(&self->leds[i + q], 0, 0, 0);        //turn every third pixel off
                    } else {
                        if (self->ledNum - 1 - (i + q) < 0) {
                            break;
                        }
                        setPixel(&self->leds[self->ledNum - 1 - (i + q)], 0, 0, 0);        //turn every third pixel off
                    }
                }
            }
        }
    }
}

[[noreturn]] void RGB_Strip::rainbow(void *pv) {
    auto *self = (RGB_Strip *) pv;
    byte c[3];

    while (true) {
        for (int j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
            for (int i = 0; i < self->ledNum; i++) {
                Wheel(((i * 256 / self->ledNum) + j) & 255, c);
                if (self->inverse) {
                    setPixel(&self->leds[self->ledNum - 1 - i], *c, *(c + 1), *(c + 2));
                } else {
                    setPixel(&self->leds[i], *c, *(c + 1), *(c + 2));
                }
            }
            FastLED.show();
            delay(10);
        }
        Serial.println("rainbow");
    }
}

void RGB_Strip::turnOff() {
    Serial.println("turnoff");
    stopTask();
    RGB_Strip::setAll(0, 0, 0, this->leds, this->ledNum);
}

void RGB_Strip::stopTask() {
    if (this->taskHandle != nullptr) {
        vTaskDelete(this->taskHandle);
        this->taskHandle = nullptr;
    }
    delay(5);
}

void RGB_Strip::RGBLoop(void *pv) {
    auto *self = (RGB_Strip *) pv;

    while (true) {
        for (int k = 0; k < 200; k++) {
            setAll(k, k, k, self->leds, self->ledNum);
            delay(3);
        }
        for (int k = 200; k >= 0; k--) {
            setAll(k, k, k, self->leds, self->ledNum);
            delay(3);
        }
    }
}

void RGB_Strip::test(void *pv) {
    auto *self = (RGB_Strip *) pv;

    while (true) {
        setAll(10, 10, 10, self->leds, self->ledNum);
        delay(50);
        setAll(5, 5, 5, self->leds, self->ledNum);
        delay(50);
    }
}
