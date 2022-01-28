#include "Display.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10];
TFT_eSPI tft = TFT_eSPI(SCREEN_WIDTH, SCREEN_HEIGHT);

TaskHandle_t displayTaskHandler = nullptr;

void dispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *) &color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void touchpadRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY, 600);
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void displayInit() {
    lv_init();

    tft.begin();
    tft.setRotation(SCREEN_ROTATION);
    touch_calibrate(false);

    lv_disp_draw_buf_init(&draw_buf, buf, nullptr, SCREEN_WIDTH * 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = SCREEN_HEIGHT;
    disp_drv.ver_res = SCREEN_WIDTH;
    disp_drv.flush_cb = dispFlush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpadRead;
    lv_indev_drv_register(&indev_drv);

    // start lvgl handler task
    displayTaskRun();

    // test
    /* Create simple label */
//    String LVGL_Arduino = "Hello Arduino! ";
//    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
//    lv_obj_t *label = lv_label_create(lv_scr_act());
//    lv_label_set_text(label, LVGL_Arduino.c_str());
//    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
//
//
//    lv_obj_t *label1;
//
//    lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
//    lv_obj_add_event_cb(btn1, eventHandler, LV_EVENT_ALL, nullptr);
//    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
//
//    label1 = lv_label_create(btn1);
//    lv_label_set_text(label1, "Button");
//    lv_obj_center(label1);
//
//    lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
//    lv_obj_add_event_cb(btn2, eventHandler, LV_EVENT_ALL, nullptr);
//    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
//    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
//    lv_obj_set_height(btn2, LV_SIZE_CONTENT);
//
//    label1 = lv_label_create(btn2);
//    lv_label_set_text(label1, "Toggle");
//    lv_obj_center(label1);
    showImg();
}

bool displayTaskRun() {
    if (displayTaskHandler != nullptr) {
        vTaskDelete(displayTaskHandler);
    }
    return xTaskCreate(&displayTask, "DisplayTask", 4096, nullptr, 1, &displayTaskHandler) == pdPASS;
}

[[noreturn]] void displayTask(void *pv) {
    TickType_t lastWakeTime;
    // delayTime = 5 ms
    const TickType_t delayTick = 5 / portTICK_PERIOD_MS;
    while (true) {
        lv_timer_handler();
        vTaskDelayUntil(&lastWakeTime, delayTick);
    }
}

void eventHandler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

void touch_calibrate(bool repeat) {
    uint16_t calData[5];
    uint8_t calDataOK = 0;

    // check file system exists
    if (!SPIFFS.begin()) {
        Serial.println("Formating file system");
        SPIFFS.format();
        SPIFFS.begin();
    }

    // check if calibration file exists and size is correct
    if (SPIFFS.exists(CALIBRATION_FILE)) {
        if (repeat) {
            // Delete if we want to re-calibrate
            SPIFFS.remove(CALIBRATION_FILE);
        } else {
            fs::File f = SPIFFS.open(CALIBRATION_FILE, "r");
            if (f) {
                if (f.readBytes((char *) calData, 14) == 14)
                    calDataOK = 1;
                f.close();
            }
        }
    }

    if (calDataOK && !repeat) {
        // calibration data valid
        tft.setTouch(calData);
    } else {
        // data not valid so recalibrate
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(20, 0);
        tft.setTextFont(2);
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);

        tft.println("Touch corners as indicated");

        tft.setTextFont(1);
        tft.println();

        if (repeat) {
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.println("Set REPEAT_CAL to false to stop this running again!");
        }

        tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Calibration complete!");

        // store data
        fs::File f = SPIFFS.open(CALIBRATION_FILE, "w");
        if (f) {
            f.write((const unsigned char *) calData, 14);
            f.close();
        }
    }
}

void showImg() {
    LV_IMG_DECLARE(img1_src)
    lv_obj_t *img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &img1_src);
}
