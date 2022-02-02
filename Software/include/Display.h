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

void touch_calibrate(bool repeat);

void showMainPage();

void testMenu();

// lvgl fs callback
static void * openSpiffsFile (lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t closeSpiffsFile (lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t readSpiffsFile (lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t seekSpiffsFile (lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t tellSpiffsFile (lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

#endif //SOFTWARE_DISPLAY_H
