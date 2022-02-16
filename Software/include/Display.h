#ifndef SOFTWARE_DISPLAY_H
#define SOFTWARE_DISPLAY_H

#include <TFT_eSPI.h>
#include <lvgl.h>
#include <freertos/FreeRTOS.h>
#include "FS.h"
#include <Arduino.h>
#include "../Config.h"
#include "Network.h"
#include "Claw.h"
#include "Setup.h"
#include "RTC.h"
#include "music.h"

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

void click_grab_button(lv_event_t *e);

void click_menu(lv_event_t *e);

void click_back(lv_event_t *e);

void change_wifi_switch(lv_event_t *e);

void click_wifi_refresh(lv_event_t *e);

void click_wifi_item(lv_event_t *e);

void click_close_wifi_connect_win(lv_event_t *e);

void click_wifi_connect(lv_event_t *e);

void click_wifi_disconnect(lv_event_t *e);

void click_grab_start(lv_event_t *e);

void click_grab_finish(lv_event_t *e);


// UI update function
void UI_update_wifi_state();

void UI_refresh_wifi_list(void *pv);

void UI_connect_wifi(void *pv);

void UI_turn_on_wifi_task(void *pv);

void UI_update_grab_time(void *pv);

[[noreturn]] void UI_update_datetime(void *pv);

void UI_update_grab_num(void *pv);

void UI_update_audio_state();


// screen load function
void mainScreenInit();

void settingScreenInit();

void grabScreenInit();

#endif //SOFTWARE_DISPLAY_H
