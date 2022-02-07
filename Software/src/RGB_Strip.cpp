#include "RGB_Strip.h"

SemaphoreHandle_t RGB_Strip::refreshMutex = xSemaphoreCreateMutex();

RGB_Strip::RGB_Strip(int id) : id(id) {
    String("RGB" + String(id)).toCharArray(name, 5);
    switch (id) {
        case 1:
            dataPin = PIN_RGB1;
            ledNum = RGB1_NUM_LEDS;
            break;
        case 2:
            dataPin = PIN_RGB2;
            ledNum = RGB2_NUM_LEDS;
            break;
        case 3:
            dataPin = PIN_RGB3;
            ledNum = RGB3_NUM_LEDS;
            break;
        case 4:
            dataPin = PIN_RGB4;
            ledNum = RGB4_NUM_LEDS;
            break;
        default:
            Serial.println("Error! Please set RGB id:" + String(id) + "in /src/RGB.cpp init function!");
    }
    leds = new CRGB[ledNum];

    switch (dataPin) {
        case 1:
            this->controller = &CFastLED::addLeds<NEOPIXEL, 1>(leds, ledNum);
            break;
        case 23:
            this->controller = &CFastLED::addLeds<NEOPIXEL, 23>(leds, ledNum);
            break;
        case 25:
            this->controller = &CFastLED::addLeds<NEOPIXEL, 25>(leds, ledNum);
            break;
        case 27:
            this->controller = &CFastLED::addLeds<NEOPIXEL, 27>(leds, ledNum);
            break;
        default:
            Serial.println("Error! Please set dataPin:" + String(dataPin) + "in /src/RGB.cpp init function!");
    }

    xTaskCreatePinnedToCore(&RGB_Strip::task, this->name, 1024, this, 1, nullptr, 1);
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

void RGB_Strip::refresh(RGB_Strip *strip) {
    xSemaphoreTake(RGB_Strip::refreshMutex, portMAX_DELAY);
    for (int i = 0; i < STRIP_NUM; ++i) {
        strip->controller->showLeds(strip->brightness);
    }
    xSemaphoreGive(RGB_Strip::refreshMutex);
}

void RGB_Strip::setEffect(int rgbEffect) {
    this->effect = rgbEffect;
    this->changeEffect = true;
}

void RGB_Strip::task(void *pv) {
    auto *self = (RGB_Strip *) pv;

    while (true) {
        self->changeEffect = false;
        switch (self->effect) {
            case 0:
                RGB_Strip::off(self);
                break;
            case 1:
                RGB_Strip::rainbow(self);
                break;
            case 2:
                RGB_Strip::theaterRainbow(self);
                break;
            default:
                RGB_Strip::off(self);
        }
    }
}

void RGB_Strip::off(RGB_Strip *self) {
    TickType_t lastWakeTime;

    while (!self->changeEffect) {
        RGB_Strip::setAll(0, 0, 0, self->leds, self->ledNum);
        RGB_Strip::refresh(self);
        vTaskDelayUntil(&lastWakeTime, 100 / portTICK_PERIOD_MS);
    }
}

void RGB_Strip::rainbow(RGB_Strip *self) {
    byte c[3];
    TickType_t lastWakeTime;

    while (!self->changeEffect) {
        for (int j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
            for (int i = 0; i < self->ledNum; i++) {
                Wheel(((i * 256 / self->ledNum) + j) & 255, c);
                if (self->inverse) {
                    setPixel(&self->leds[self->ledNum - 1 - i], *c, *(c + 1), *(c + 2));
                } else {
                    setPixel(&self->leds[i], *c, *(c + 1), *(c + 2));
                }
            }
            RGB_Strip::refresh(self);
            if (self->changeEffect) {
                break;
            }
            vTaskDelayUntil(&lastWakeTime, self->rainbowDelay / portTICK_PERIOD_MS);
        }
    }
}

void RGB_Strip::theaterRainbow(RGB_Strip *self) {
    byte c[3];
    TickType_t lastWakeTime;

    while (!self->changeEffect) {
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
                RGB_Strip::refresh(self);
                if (self->changeEffect) {
                    break;
                }
                vTaskDelayUntil(&lastWakeTime, self->theaterDelay / portTICK_PERIOD_MS);

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
            if (self->changeEffect) {
                break;
            }
        }
    }
}
