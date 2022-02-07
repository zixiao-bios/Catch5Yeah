#ifndef SOFTWARE_CONFIG_H
#define SOFTWARE_CONFIG_H

/**======================
 * 1. ESP32 pin settings
 **======================*/

// Display pin defined in ./platformio.ini

// I2S pin
#define PIN_I2S_LRC 13
#define PIN_I2S_BLCK 12
#define PIN_I2S_DIN 14

// RGB pin
#define PIN_RGB1 23
#define PIN_RGB2 1
#define PIN_RGB3 25
#define PIN_RGB4 27

// keypad, button
#define PIN_K1 26
#define PIN_K2 3
#define PIN_K3 32
#define PIN_K4 33
#define PIN_BTN 35

// shift register
#define PIN_SR_SCK 0
#define PIN_SR_RCK 2
#define PIN_SR_SI 15

// proximity switch
#define PIN_S1 36
#define PIN_S2 39
#define PIN_S3 34


/**======================
 * 2. Display & Touch screen
 **======================*/
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 480
#define SCREEN_WIDTH DISPLAY_HEIGHT
#define SCREEN_HEIGHT DISPLAY_WIDTH
#define SCREEN_ROTATION 3
#define CALIBRATION_FILE "/TouchCalData"

// colors
#define COLOR_GOOD 0x00cc00
#define COLOR_NORMAL 0xFF7800
#define COLOR_BAD 0xff0000


/**======================
 * 3. RGB Strip
 **======================*/

// settings
#define STRIP_NUM 4
#define RGB_BRIGHTNESS 128
#define RGB1_NUM_LEDS 111
#define RGB2_NUM_LEDS 111
#define RGB3_NUM_LEDS 113
#define RGB4_NUM_LEDS 41

// RGB effect definition
# define RGB_OFF 0
# define RGB_RAINBOW 1
# define RGB_THEATER_RAINBOW 2

#endif //SOFTWARE_CONFIG_H
