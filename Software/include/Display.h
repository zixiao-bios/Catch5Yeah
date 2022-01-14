#ifndef SOFTWARE_DISPLAY_H
#define SOFTWARE_DISPLAY_H

#include <TFT_eSPI.h>
#include <lvgl.h>
#include <freertos/FreeRTOS.h>

    void display();

    bool run();

    void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

    void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

    [[noreturn]] void displayTask(void *pv);

    void event_handler(lv_event_t *e);

#endif //SOFTWARE_DISPLAY_H
