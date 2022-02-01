#ifndef SOFTWARE_CONFIG_H
#define SOFTWARE_CONFIG_H

/**======================
 * 1. ESP32 pin settings
 **======================*/

// Display pin defined in ./platformio.ini

// I2S pin
#define I2S_LRC 13
#define I2S_BLCK 12
#define I2S_DIN 14

// RGB pin
#define RGB1 23
#define RGB2 1
#define RGB3 25
#define RGB4 27
#define RGB1_NUM_LEDS 111
#define RGB2_NUM_LEDS 111
#define RGB3_NUM_LEDS 113
#define RGB4_NUM_LEDS 16

// keypad, button
#define K1 26
#define K2 3
#define K3 32
#define K4 33
#define BTN 35


/**======================
 * 2. Display & Touch screen
 **======================*/
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 480
#define SCREEN_WIDTH DISPLAY_HEIGHT
#define SCREEN_HEIGHT DISPLAY_WIDTH
#define SCREEN_ROTATION 3
#define CALIBRATION_FILE "/TouchCalData"


/**======================
 * 3. RGB Strip
 **======================*/

// settings
#define STRIP_NUM 2
#define RGB_BRIGHTNESS 128

// RGB effect definition
# define RGB_OFF 0
# define RGB_RAINBOW 1
# define RGB_THEATER_RAINBOW 2

#endif //SOFTWARE_CONFIG_H
