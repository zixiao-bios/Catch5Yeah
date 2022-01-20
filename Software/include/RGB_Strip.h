#ifndef SOFTWARE_RGB_STRIP_H
#define SOFTWARE_RGB_STRIP_H


#include <Arduino.h>
#include <FastLED.h>
#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include "config.h"

class RGB_Strip {
public:
    // RGB effect rotation direction
    bool inverse = false;

    int brightness = RGB_BRIGHTNESS;
    int rainbowDelay = 10;
    int theaterDelay = 20;

    static SemaphoreHandle_t refreshMutex;

    explicit RGB_Strip(int id);

    void setEffect(int rgbEffect);

private:
    int id, dataPin, ledNum;
    CRGB *leds;
    int effect = 0;
    CLEDController *controller;
    char name[5]{};
    bool changeEffect = false;

    static void refresh(RGB_Strip *strip);

    static void setPixel(CRGB *Pixel, byte red, byte green, byte blue);
    static void setAll(byte red, byte green, byte blue, CRGB* leds, int ledNum);

    static void Wheel(byte WheelPos, byte c[3]);

    [[noreturn]] static void task(void *pv);
    static void off(RGB_Strip *self);
    static void rainbow(RGB_Strip *self);
    static void theaterRainbow(RGB_Strip *self);
};


#endif //SOFTWARE_RGB_STRIP_H
