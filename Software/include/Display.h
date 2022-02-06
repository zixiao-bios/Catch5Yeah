#ifndef SOFTWARE_DISPLAY_H
#define SOFTWARE_DISPLAY_H

#include <TFT_eSPI.h>
#include <lvgl.h>
#include <freertos/FreeRTOS.h>
#include "FS.h"
#include <Arduino.h>
#include "../Config.h"
#include "Network.h"

extern SemaphoreHandle_t lvgl_mutex;

// lvgl init function
void displayInit();

void dispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

void touchpadRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

void touch_calibrate(bool repeat);


// show screen
void show_screen(const String& screen_name);


// lvgl callback function
void click_setting_button(lv_event_t *e);

void click_menu(lv_event_t *e);

void change_wifi_switch(lv_event_t *e);

void click_wifi_refresh(lv_event_t *e);

void click_wifi_item(lv_event_t *e);

void click_close_wifi_connect_win(lv_event_t *e);

void click_wifi_connect(lv_event_t *e);

void click_wifi_disconnect(lv_event_t *e);


// UI update function
void UI_update_wifi_state();

void UI_refresh_wifi_list(void *pv);

void UI_connect_wifi(void *pv);

void UI_turn_on_wifi(void *pv);


// screen load function
void mainScreenInit();

void settingScreenInit();


// lvgl fs callback
static void *openSpiffsFile(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode);

static lv_fs_res_t closeSpiffsFile(lv_fs_drv_t *drv, void *file_p);

static lv_fs_res_t readSpiffsFile(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);

static lv_fs_res_t seekSpiffsFile(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence);

static lv_fs_res_t tellSpiffsFile(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);

#endif //SOFTWARE_DISPLAY_H
