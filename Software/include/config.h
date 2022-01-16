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
#define RGB1 0
#define RGB2 1
#define RGB3 19
#define RGB4 27
#define RGB1_NUM_LEDS 16


/**======================
 * 2. Display settings
 **======================*/
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define SCREEN_ROTATION 3

#endif //SOFTWARE_CONFIG_H
