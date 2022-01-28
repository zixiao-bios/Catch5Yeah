#ifndef SOFTWARE_DISPLAY_H
#define SOFTWARE_DISPLAY_H

#include <TFT_eSPI.h>
#include <lvgl.h>
#include <freertos/FreeRTOS.h>
#include "FS.h"
#include "../Config.h"

    void displayInit();

    bool displayTaskRun();

    void dispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

    void touchpadRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

    [[noreturn]] void displayTask(void *pv);

    void eventHandler(lv_event_t *e);

    void touch_calibrate(bool repeat);

#endif //SOFTWARE_DISPLAY_H
