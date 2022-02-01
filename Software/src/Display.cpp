#include "Display.h"

// Display
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_buf[DISPLAY_WIDTH * 10];
TFT_eSPI tft = TFT_eSPI(DISPLAY_WIDTH, DISPLAY_HEIGHT);

// File system
static lv_fs_drv_t flashDrv;

// declare custom font
LV_FONT_DECLARE(font_middle)

TaskHandle_t displayTaskHandler = nullptr;

static void *openSpiffsFile(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode) {
    auto *file = new fs::File();
    char filename[32];
    snprintf_P(filename, sizeof(filename), PSTR("/%s"), path);
    *file = SPIFFS.open(filename, mode == LV_FS_MODE_WR ? FILE_WRITE : FILE_READ);

    if (!*file || file->isDirectory()) {
        Serial.print("failed to open file: ");
        Serial.println(path);
        return nullptr;
    }

    return file;
}

static lv_fs_res_t closeSpiffsFile(lv_fs_drv_t *drv, void *file_p) {
    fs::File file = *(fs::File *) file_p;
    if (!file) {
        return LV_FS_RES_NOT_EX;

    } else if (file.isDirectory()) {
        return LV_FS_RES_UNKNOWN;

    } else {
        file.close();
        return LV_FS_RES_OK;
    }
}

static lv_fs_res_t readSpiffsFile(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br) {
    auto *fp = (fs::File *) file_p;
    fs::File file = *fp;

    if (!file) {
        return LV_FS_RES_NOT_EX;
    } else {
        *br = (uint32_t) file.readBytes((char *) buf, btr);
        return LV_FS_RES_OK;
    }
}

static lv_fs_res_t seekSpiffsFile(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence) {
    fs::File file = *(fs::File *) file_p;

    if (!file) {
        return LV_FS_RES_NOT_EX;
    } else {
        file.seek(pos, fs::SeekSet);
        return LV_FS_RES_OK;
    }
}

static lv_fs_res_t tellSpiffsFile(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p) {
    fs::File file = *(fs::File *) file_p;

    if (!file) {
        return LV_FS_RES_NOT_EX;
    } else {
        *pos_p = (uint32_t) file.position();
        return LV_FS_RES_OK;
    }
}

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

    lv_disp_draw_buf_init(&draw_buf, disp_buf, nullptr, DISPLAY_WIDTH * 10);

    // init display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = DISPLAY_HEIGHT;
    disp_drv.ver_res = DISPLAY_WIDTH;
    disp_drv.flush_cb = dispFlush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // init touch screen
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpadRead;
    lv_indev_drv_register(&indev_drv);

    // init file system
    lv_fs_drv_init(&flashDrv);
    flashDrv.letter = 'F';
//    flashDrv.cache_size = 0;
    flashDrv.ready_cb = nullptr;
    flashDrv.open_cb = openSpiffsFile;
    flashDrv.close_cb = closeSpiffsFile;
    flashDrv.read_cb = readSpiffsFile;
    flashDrv.write_cb = nullptr;
    flashDrv.seek_cb = seekSpiffsFile;
    flashDrv.tell_cb = tellSpiffsFile;
    flashDrv.dir_close_cb = nullptr;
    flashDrv.dir_open_cb = nullptr;
    flashDrv.dir_read_cb = nullptr;
    lv_fs_drv_register(&flashDrv);

    // start lvgl handler task
    displayTaskRun();

    showMainPage();
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
        Serial.println("<");
        lv_timer_handler();
        Serial.println(">");
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

void showMainPage() {
    // background
    lv_obj_t *bg_box1 = lv_obj_create(lv_scr_act());
    lv_coord_t bg_box1_height = 100;
    lv_obj_set_size(bg_box1, SCREEN_WIDTH, bg_box1_height);
    lv_obj_set_align(bg_box1, LV_ALIGN_TOP_MID);
    lv_obj_set_style_bg_color(bg_box1, lv_color_hex(0x2F3243), 0);
    lv_obj_set_style_border_width(bg_box1, 0, 0);
    lv_obj_set_style_radius(bg_box1, 0, 0);

    lv_obj_t *bg_box2 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(bg_box2, 480, SCREEN_HEIGHT - bg_box1_height);
    lv_obj_set_align(bg_box2, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_bg_color(bg_box2, lv_color_hex(0xF3F8FE), 0);
    lv_obj_set_style_border_width(bg_box2, 0, 0);
    lv_obj_set_style_radius(bg_box2, 0, 0);

    // status bar
    // icon
    lv_obj_t *network_img = lv_img_create(lv_scr_act());
    lv_img_set_src(network_img, "F:icn_NoNetwork.bin");
    lv_obj_set_pos(network_img, 30, 28);

    // datetime
    lv_obj_t *datetime_label = lv_label_create(lv_scr_act());
    lv_label_set_text(datetime_label, "2022年2月1日 23:47");
    lv_obj_set_align(datetime_label, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(datetime_label, -30, 28);
    lv_obj_set_style_text_color(datetime_label, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(datetime_label, &font_middle, 0);
}
