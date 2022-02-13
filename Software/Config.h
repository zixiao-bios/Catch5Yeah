#ifndef SOFTWARE_CONFIG_H
#define SOFTWARE_CONFIG_H

/**======================
 * 1. ESP32 pin settings
 **======================*/

// Display pin defined in ./platformio.ini

// I2S pin
#define PIN_I2S_LRC 14
#define PIN_I2S_BLCK 12
#define PIN_I2S_DIN 13

// RTC pin
#define PIN_RTC_SDA 19
#define PIN_RTC_SCL 16

// RGB pin
#define PIN_RGB1 23
#define PIN_RGB2 1
#define PIN_RGB3 25
#define PIN_RGB4 27

// keypad, button
#define PIN_KEY_DOWN 26
#define PIN_KEY_UP 3
#define PIN_KEY_RIGHT 32
#define PIN_KEY_LEFT 33
#define PIN_BTN 35

// shift register
#define PIN_SR_SCK 0
#define PIN_SR_RCK 2
#define PIN_SR_SI 15

// motor, electromagnet
#define SR_PIN_M1A 1
#define SR_PIN_M1B 2
#define SR_PIN_M2A 3
#define SR_PIN_M2B 4
#define SR_PIN_M3 5
#define SR_PIN_MAG 6

// proximity switch
#define PIN_S_RIGHT 36
#define PIN_S_LEFT 39
#define PIN_S_UP 34

// screen backlight
#define SR_PIN_SCREEN 7


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
#define STRIP_NUM 3
#define RGB_BRIGHTNESS 128
#define RGB1_NUM_LEDS 111
#define RGB2_NUM_LEDS 111
#define RGB3_NUM_LEDS 113
#define RGB4_NUM_LEDS 41

// RGB effect definition
# define RGB_OFF 0
# define RGB_RAINBOW 1
# define RGB_THEATER_RAINBOW 2


/**======================
 * 4. Datetime
 **======================*/
#define DATETIME_BEGIN_YEAR 2016
#define DATETIME_BEGIN_MONTH 12
#define DATETIME_BEGIN_DAY 25
#define DATETIME_BEGIN_HOUR 21
#define DATETIME_BEGIN_MIN 50

#endif //SOFTWARE_CONFIG_H
