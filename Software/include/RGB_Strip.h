#ifndef SOFTWARE_RGB_STRIP_H
#define SOFTWARE_RGB_STRIP_H


#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

class RGB_Strip {
public:
    // RGB effect rotation direction
    bool inverse = false;

    explicit RGB_Strip(int id);

    bool setEffect(const String& effect);

private:
    int id, dataPin, ledNum;
    CRGB *leds;
    TaskHandle_t taskHandle = nullptr;
    char name[5]{};

    static void setPixel(CRGB *Pixel, byte red, byte green, byte blue);
    static void setAll(byte red, byte green, byte blue, CRGB* leds, int ledNum);

    static void Wheel(byte WheelPos, byte c[3]);

    [[noreturn]] static void rainbow(void *pv);
    [[noreturn]] static void theaterRainbow(void *pv);
};


#endif //SOFTWARE_RGB_STRIP_H
