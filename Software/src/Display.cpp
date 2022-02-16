#include <Audio.h>
#include "Display.h"

// Display
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_buf[SCREEN_WIDTH * 10];
TFT_eSPI tft = TFT_eSPI(DISPLAY_WIDTH, DISPLAY_HEIGHT);

// File system
static lv_fs_drv_t flashDrv;

// declare custom font
LV_FONT_DECLARE(font_middle)
LV_FONT_DECLARE(font_small)

// declare imgs
LV_IMG_DECLARE(icn_Claw)
LV_IMG_DECLARE(icn_News)
LV_IMG_DECLARE(icn_Settings)
LV_IMG_DECLARE(icn_Network)
LV_IMG_DECLARE(icn_NoNetwork)
LV_IMG_DECLARE(icn_WiFi)
LV_IMG_DECLARE(icn_Sound)
LV_IMG_DECLARE(icn_RGB)
LV_IMG_DECLARE(icn_Mute)
LV_IMG_DECLARE(icn_Music)
LV_IMG_DECLARE(icn_Volume)
LV_IMG_DECLARE(img_DST)
LV_IMG_DECLARE(img_GrabPage)

// screens
lv_obj_t *main_screen, *setting_screen, *grab_screen;

// widgets
// main screen
lv_obj_t *main_network_img, *main_datetime_label;

// setting screen
// wifi page
lv_obj_t *wifi_page, *wifi_switch, *wifi_state_section, *wifi_connect_state_label, *wifi_disconnect_button, *wifi_list_label, *wifi_list_section, *wifi_refresh_button, *wifi_connect_win_bg;
String *wifi_name_clicked = nullptr;
// audio page
lv_obj_t *audio_mute_switch, *audio_volume_section, *audio_music_section, *audio_volume_label, *audio_music_switch;

// grab screen
lv_obj_t *grab_label, *grab_back_button, *grab_start_button, *grab_finish_button;

SemaphoreHandle_t lvgl_mutex;


static void *openFile(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode) {
    auto *file = new fs::File();
    char filename[32];
    snprintf_P(filename, sizeof(filename), PSTR("/%s"), path);
    *file = LITTLEFS.open(filename, mode == LV_FS_MODE_WR ? FILE_WRITE : FILE_READ);

    if (!*file || file->isDirectory()) {
        Serial.print("failed to open file: ");
        Serial.println(path);
        return nullptr;
    }

    return file;
}

static lv_fs_res_t closeFile(lv_fs_drv_t *drv, void *file_p) {
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

static lv_fs_res_t readFile(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br) {
    auto *fp = (fs::File *) file_p;
    fs::File file = *fp;

    if (!file) {
        return LV_FS_RES_NOT_EX;
    } else {
        *br = (uint32_t) file.readBytes((char *) buf, btr);
        return LV_FS_RES_OK;
    }
}

static lv_fs_res_t seekFile(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence) {
    fs::File file = *(fs::File *) file_p;

    if (!file) {
        return LV_FS_RES_NOT_EX;
    } else {
        file.seek(pos, fs::SeekSet);
        return LV_FS_RES_OK;
    }
}

static lv_fs_res_t tellFile(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p) {
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
    lvgl_mutex = xSemaphoreCreateMutex();
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
    flashDrv.cache_size = 0;
    flashDrv.ready_cb = nullptr;
    flashDrv.open_cb = openFile;
    flashDrv.close_cb = closeFile;
    flashDrv.read_cb = readFile;
    flashDrv.write_cb = nullptr;
    flashDrv.seek_cb = seekFile;
    flashDrv.tell_cb = tellFile;
    flashDrv.dir_close_cb = nullptr;
    flashDrv.dir_open_cb = nullptr;
    flashDrv.dir_read_cb = nullptr;
    lv_fs_drv_register(&flashDrv);

    mainScreenInit();
    settingScreenInit();
    grabScreenInit();
    xTaskCreatePinnedToCore(UI_update_datetime, "Datetime", 2048, nullptr, 2, nullptr, 1);
}

void touch_calibrate(bool repeat) {
    uint16_t calData[5];
    uint8_t calDataOK = 0;

    // check if calibration file exists and size is correct
    if (LITTLEFS.exists(CALIBRATION_FILE)) {
        if (repeat) {
            // Delete if we want to re-calibrate
            LITTLEFS.remove(CALIBRATION_FILE);
        } else {
            fs::File f = LITTLEFS.open(CALIBRATION_FILE, "r");
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
        fs::File f = LITTLEFS.open(CALIBRATION_FILE, "w");
        if (f) {
            f.write((const unsigned char *) calData, 14);
            f.close();
        }
    }
}

void show_screen(const String &screen_name) {
    if (screen_name == "main") {
        lv_scr_load(main_screen);
    } else if (screen_name == "setting") {
        lv_scr_load(setting_screen);
    } else if (screen_name == "grab") {
        lv_scr_load(grab_screen);
    }
}

void click_setting_button(lv_event_t *e) {
    show_screen("setting");
}

void click_grab_button(lv_event_t *e) {
    lv_obj_add_state(grab_start_button, LV_STATE_DISABLED);
    if (wifi_connect_state()) {
        lv_label_set_text(grab_label, "正在获取可用次数...");
        xTaskCreatePinnedToCore(UI_update_grab_num, "UpdateGrabNum", 4096, nullptr, 1, nullptr, 0);
    } else {
        lv_label_set_text(grab_label, "请先连接网络");
    }
    show_screen("grab");
}

void click_menu(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    auto *menu = (lv_obj_t *) lv_event_get_user_data(e);

    if (lv_menu_back_btn_is_root(menu, obj)) {
        show_screen("main");
    }
}

void click_back(lv_event_t *e) {
    show_screen("main");
}

void change_wifi_switch(lv_event_t *e) {
    if (lv_obj_has_state(wifi_switch, LV_STATE_CHECKED)) {
        // turn on wifi
        xTaskCreatePinnedToCore(UI_turn_on_wifi_task, "TurnOnWifi", 10000, nullptr, 1, nullptr, 0);

        // update setup
        setup_doc["wifi"]["enable"] = true;
        write_setup();
    } else {
        // turn off wifi
        wifi_off();
        if (wifi_list_section) {
            lv_obj_del(wifi_list_section);
            wifi_list_section = nullptr;
        }
        UI_update_wifi_state();

        // update setup
        setup_doc["wifi"]["enable"] = false;
        write_setup();
    }
}

void click_wifi_refresh(lv_event_t *e) {
    xTaskCreatePinnedToCore(UI_refresh_wifi_list, "RefreshWifiList", 2048, nullptr, 1, nullptr, 0);
}

void click_wifi_item(lv_event_t *e) {
    lv_obj_t *cont = lv_obj_get_parent(lv_event_get_target(e));
    delete wifi_name_clicked;
    wifi_name_clicked = new String(lv_label_get_text(lv_obj_get_child(cont, 0)));

    // open Wi-Fi connect window
    wifi_connect_win_bg = lv_obj_create(setting_screen);
    lv_obj_center(wifi_connect_win_bg);
    lv_obj_set_size(wifi_connect_win_bg, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(wifi_connect_win_bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(wifi_connect_win_bg, LV_OPA_50, 0);
    lv_obj_set_style_border_width(wifi_connect_win_bg, 0, 0);
    lv_obj_set_style_radius(wifi_connect_win_bg, 0, 0);
    lv_obj_set_style_pad_all(wifi_connect_win_bg, 0, 0);

    lv_obj_t *mbox = lv_msgbox_create(wifi_connect_win_bg, String("连接到 " + *wifi_name_clicked).c_str(), nullptr,
                                      nullptr, false);
    lv_obj_set_style_text_font(mbox, &font_small, 0);
    lv_obj_add_flag(lv_msgbox_get_content(mbox), LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_align(mbox, LV_ALIGN_TOP_MID);

    lv_obj_t *pwd_ta = lv_textarea_create(mbox);
    lv_textarea_set_text(pwd_ta, "");
    lv_textarea_set_placeholder_text(pwd_ta, "请输入密码");
    lv_textarea_set_password_mode(pwd_ta, true);
    lv_textarea_set_one_line(pwd_ta, true);
    lv_obj_set_width(pwd_ta, 220);

    lv_obj_t *connect_btn = lv_btn_create(mbox);
    lv_obj_set_style_text_color(connect_btn, lv_color_white(), 0);
    lv_obj_set_style_bg_color(connect_btn, lv_color_hex(COLOR_GOOD), 0);
    lv_obj_t *label = lv_label_create(connect_btn);
    lv_label_set_text(label, "连接");
    lv_obj_set_style_pad_all(label, -4, 0);
    lv_obj_add_event_cb(connect_btn, click_wifi_connect, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *cancel_button = lv_btn_create(mbox);
    lv_obj_set_style_text_color(cancel_button, lv_color_white(), 0);
    lv_obj_set_style_bg_color(cancel_button, lv_color_hex(COLOR_NORMAL), 0);
    label = lv_label_create(cancel_button);
    lv_label_set_text(label, "取消");
    lv_obj_set_style_pad_all(label, -4, 0);
    lv_obj_add_event_cb(cancel_button, click_close_wifi_connect_win, LV_EVENT_CLICKED, nullptr);

    label = lv_label_create(mbox);
    lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);


    lv_obj_t *kb = lv_keyboard_create(wifi_connect_win_bg);
    lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);
    lv_keyboard_set_textarea(kb, pwd_ta);
}

void click_close_wifi_connect_win(lv_event_t *e) {
    lv_obj_del(wifi_connect_win_bg);
    wifi_connect_win_bg = nullptr;
}

void click_wifi_connect(lv_event_t *e) {
    xTaskCreatePinnedToCore(UI_connect_wifi, "wifi_connect", 4096, e, 1, nullptr, 1);
}

void click_wifi_disconnect(lv_event_t *e) {
    wifi_disconnect();
    if (wifi_list_section) {
        lv_obj_del(wifi_list_section);
        wifi_list_section = nullptr;
    }
    delay(100);
    UI_update_wifi_state();
}

void click_grab_start(lv_event_t *e) {
    lv_obj_add_flag(grab_back_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(grab_start_button, LV_OBJ_FLAG_HIDDEN);

    play_one_loop();

    claw_grab_start();

    xTaskCreatePinnedToCore(UI_update_grab_time, "GrabTimeUpdate", 4096, nullptr, 1, nullptr, 1);
}

void click_grab_finish(lv_event_t *e) {
    claw_grab_exit();

    play_stop();

    show_screen("main");

    lv_obj_clear_flag(grab_back_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(grab_start_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(grab_finish_button, LV_OBJ_FLAG_HIDDEN);
}

void UI_update_wifi_state() {
    if (wifi_on_state()) {
        lv_obj_add_state(wifi_switch, LV_STATE_CHECKED);
        lv_obj_clear_flag(wifi_state_section, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_state(wifi_switch, LV_STATE_CHECKED);
        lv_obj_add_flag(wifi_state_section, LV_OBJ_FLAG_HIDDEN);
    }

    if (wifi_connect_state()) {
        // update wifi icon
        lv_label_set_text(wifi_connect_state_label, wifi_name_state().c_str());
        lv_obj_clear_flag(wifi_disconnect_button, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(main_network_img, &icn_Network);
    } else {
        lv_label_set_text(wifi_connect_state_label, "无");
        lv_obj_add_flag(wifi_disconnect_button, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(main_network_img, &icn_NoNetwork);
    }

    if (wifi_on_state() && !wifi_connect_state()) {
        lv_obj_clear_flag(wifi_list_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(wifi_refresh_button, LV_OBJ_FLAG_HIDDEN);
        if (wifi_list_section) {
            lv_obj_clear_flag(wifi_list_section, LV_OBJ_FLAG_HIDDEN);
        }
    } else {
        lv_obj_add_flag(wifi_list_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(wifi_refresh_button, LV_OBJ_FLAG_HIDDEN);
        if (wifi_list_section) {
            lv_obj_add_flag(wifi_list_section, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void UI_refresh_wifi_list(void *pv) {
    int n = scanWiFiList();
    if (n > 20) {
        n = 20;
    }

    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);

    if (wifi_list_section) {
        lv_obj_del(wifi_list_section);
        wifi_list_section = nullptr;
    }

    lv_obj_t *cont, *obj;
    wifi_list_section = lv_menu_section_create(wifi_page);

    int rssi;
    for (int i = 0; i < n; ++i) {
        cont = lv_menu_cont_create(wifi_list_section);

        obj = lv_label_create(cont);
        lv_label_set_text(obj, WiFi.SSID(i).c_str());
        lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(obj, 1);

        obj = lv_label_create(cont);
        rssi = WiFi.RSSI(i);
        if (rssi > -70) {
            lv_label_set_text(obj, "强");
            lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_GOOD), 0);
        } else if (rssi > -90) {
            lv_label_set_text(obj, "中");
            lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_NORMAL), 0);
        } else {
            lv_label_set_text(obj, "弱");
            lv_obj_set_style_text_color(obj, lv_color_hex(COLOR_BAD), 0);
        }

        obj = lv_btn_create(cont);
        lv_obj_set_style_bg_color(obj, lv_color_hex(COLOR_GOOD), 0);
        lv_obj_add_event_cb(obj, click_wifi_item, LV_EVENT_CLICKED, nullptr);
        lv_obj_set_style_text_color(obj, lv_color_white(), 0);

        obj = lv_label_create(obj);
        lv_label_set_text(obj, "连接");
        lv_obj_set_style_pad_all(obj, -7, 0);
    }

    xSemaphoreGive(lvgl_mutex);

    vTaskDelete(nullptr);
}

void UI_connect_wifi(void *pv) {
    // get Wi-Fi name and widgets
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    auto *mbox = lv_obj_get_child(wifi_connect_win_bg, 0);
    auto *pwd_ta = lv_obj_get_child(mbox, -4);
    auto *connect_btn = lv_obj_get_child(mbox, -3);
    auto *cancel_btn = lv_obj_get_child(mbox, -2);
    auto *label = lv_obj_get_child(mbox, -1);
    String password = lv_textarea_get_text(pwd_ta);

    // show "connecting..." and hidden buttons
    lv_label_set_text(label, "正在连接...");
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_NORMAL), 0);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(connect_btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(cancel_btn, LV_OBJ_FLAG_HIDDEN);
    xSemaphoreGive(lvgl_mutex);

    // connect wifi
    if (wifi_connect(*wifi_name_clicked, password)) {
        // show "success"
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        lv_label_set_text(label, "连接成功！");
        lv_obj_set_style_text_color(label, lv_color_hex(COLOR_GOOD), 0);
        xSemaphoreGive(lvgl_mutex);

        // update setup
        setup_doc["wifi"]["name"] = *wifi_name_clicked;
        setup_doc["wifi"]["password"] = password;
        write_setup();

        delay(2000);

        // exit window
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        click_close_wifi_connect_win(nullptr);
    } else {
        // show "fail"
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        lv_label_set_text(label, "连接失败，请重试！");
        lv_obj_set_style_text_color(label, lv_color_hex(COLOR_BAD), 0);
        xSemaphoreGive(lvgl_mutex);

        delay(2000);

        // show buttons
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(connect_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(cancel_btn, LV_OBJ_FLAG_HIDDEN);
    }

    UI_update_wifi_state();
    xSemaphoreGive(lvgl_mutex);

    vTaskDelete(nullptr);
}

void UI_turn_on_wifi_task(void *pv) {
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    lv_obj_add_state(wifi_switch, LV_STATE_CHECKED);
    lv_obj_add_state(wifi_switch, LV_STATE_DISABLED);
    xSemaphoreGive(lvgl_mutex);

    wifi_on();

    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    lv_obj_clear_state(wifi_switch, LV_STATE_DISABLED);
    UI_update_wifi_state();
    xSemaphoreGive(lvgl_mutex);
    vTaskDelete(nullptr);
}

void UI_update_grab_time(void *pv) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    delay(100);

    for (int time = 60; time > 0; time--) {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        lv_label_set_text(grab_label, String("剩余时间：" + String(time) + "秒").c_str());
        xSemaphoreGive(lvgl_mutex);

        if (claw_get_move_done()) {
            // move done (push button)
            break;
        }

        vTaskDelayUntil(&lastWakeTime, 1000 / portTICK_PERIOD_MS);
    }

    // check if move is done
    if (!claw_get_move_done()) {
        // timeout
        claw_grab_timeout();
    }

    // move done
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    lv_label_set_text(grab_label, "按下按钮，抓取礼物");
    xSemaphoreGive(lvgl_mutex);

    // wait player push button second time
    while (!claw_get_grab_done()) {
        delay(100);
    }

    // grab is done, wait for gift done
    // update grab num
    record_grab_once();
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    lv_label_set_text(grab_label, "正在运送礼物...");
    xSemaphoreGive(lvgl_mutex);
    while (!claw_get_gift_done()) {
        delay(100);
    }

    // gift done
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    lv_label_set_text(grab_label, "请拿取礼物");
    lv_obj_clear_flag(grab_finish_button, LV_OBJ_FLAG_HIDDEN);
    xSemaphoreGive(lvgl_mutex);

    vTaskDelete(nullptr);
}

[[noreturn]] void UI_update_datetime(void *pv) {
    while (true) {
        DateTime now = rtc_get_time();
        String year = String(now.year());
        String month = String(now.month());
        String day = String(now.day());
        String hour = String(now.hour());
        String min = String(now.minute());
        String second = String(now.second());
        if (hour.length() == 1) {
            hour = "0" + hour;
        }
        if (min.length() == 1) {
            min = "0" + min;
        }
        if (second.length() == 1) {
            second = "0" + second;
        }
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        lv_label_set_text(main_datetime_label,
                          String(year + "年" + month + "月" + day + "日 " + hour + ":" + min + ":" + second).c_str());
        xSemaphoreGive(lvgl_mutex);
        delay(1000);
    }
}

void UI_update_grab_num(void *pv) {
    int available = network_get_grab();
    int grab = get_grab_num();

    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    if (available < 0) {
        lv_label_set_text(grab_label, "服务器错误！");
    } else if (grab >= available) {
        lv_label_set_text(grab_label, "今日次数已用完");
    } else {
        lv_label_set_text(grab_label, String("今日剩余次数：" + String(available - grab)).c_str());
        lv_obj_clear_state(grab_start_button, LV_STATE_DISABLED);
    }
    xSemaphoreGive(lvgl_mutex);

    vTaskDelete(nullptr);
}

void UI_update_audio_state() {
    if (setup_doc["audio"]["mute"]) {

    }
}

void mainScreenInit() {
    main_screen = lv_obj_create(nullptr);

    // background
    lv_obj_t *bg_box1 = lv_obj_create(main_screen);
    lv_coord_t bg_box1_height = 70;
    lv_obj_set_size(bg_box1, SCREEN_WIDTH, bg_box1_height);
    lv_obj_set_align(bg_box1, LV_ALIGN_TOP_MID);
    lv_obj_set_style_bg_color(bg_box1, lv_color_hex(0x2F3243), 0);
    lv_obj_set_style_border_width(bg_box1, 0, 0);
    lv_obj_set_style_radius(bg_box1, 0, 0);

    lv_obj_t *bg_box2 = lv_obj_create(main_screen);
    lv_obj_set_size(bg_box2, 480, SCREEN_HEIGHT - bg_box1_height);
    lv_obj_set_align(bg_box2, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_bg_color(bg_box2, lv_color_white(), 0);
    lv_obj_set_style_border_width(bg_box2, 0, 0);
    lv_obj_set_style_radius(bg_box2, 0, 0);

    // status bar pos
    lv_coord_t status_pos_y = 20;

    // status bar icon
    main_network_img = lv_img_create(main_screen);
    lv_img_set_src(main_network_img, &icn_NoNetwork);
    lv_obj_set_pos(main_network_img, 30, status_pos_y);

    // status bar datetime
    main_datetime_label = lv_label_create(main_screen);
    lv_label_set_text(main_datetime_label, "");
    lv_obj_set_align(main_datetime_label, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(main_datetime_label, -30, status_pos_y);
    lv_obj_set_style_text_color(main_datetime_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(main_datetime_label, &font_middle, 0);

    // buttons style
    static lv_style_t btn_style;
    lv_style_init(&btn_style);
    lv_style_set_text_font(&btn_style, &font_middle);
    lv_style_set_text_color(&btn_style, lv_color_white());
    lv_style_set_align(&btn_style, LV_ALIGN_CENTER);
    lv_style_set_radius(&btn_style, 20);
    lv_style_set_shadow_width(&btn_style, 10);
    lv_style_set_shadow_ofs_x(&btn_style, 5);
    lv_style_set_shadow_ofs_y(&btn_style, 5);

    // buttons pos & size
    lv_coord_t button_x = 140;
    lv_coord_t button_y = 10;
    lv_coord_t button_width = 100;
    lv_coord_t button_height = 130;
    lv_coord_t button_icn_x = 5;

    // button1
    lv_obj_t *btn1 = lv_btn_create(main_screen);
    lv_obj_add_style(btn1, &btn_style, 0);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x4D55C4), 0);
    lv_obj_set_size(btn1, button_width, button_height);
    lv_obj_set_pos(btn1, -button_x, button_y);

    lv_obj_t *label1 = lv_label_create(btn1);
    lv_obj_set_align(label1, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(label1, "要闻");

    lv_obj_t *icn1 = lv_img_create(btn1);
    lv_obj_set_align(icn1, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(icn1, button_icn_x, 0);
    lv_img_set_src(icn1, &icn_News);

    // button2
    lv_obj_t *btn2 = lv_btn_create(main_screen);
    lv_obj_add_style(btn2, &btn_style, 0);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x46B147), 0);
    lv_obj_set_size(btn2, button_width, button_height);
    lv_obj_set_pos(btn2, 0, button_y);
    lv_obj_add_event_cb(btn2, click_grab_button, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *label2 = lv_label_create(btn2);
    lv_obj_set_align(label2, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(label2, "抓取");

    lv_obj_t *icn2 = lv_img_create(btn2);
    lv_obj_set_align(icn2, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(icn2, button_icn_x, 0);
    lv_img_set_src(icn2, &icn_Claw);

    // button3
    lv_obj_t *btn3 = lv_btn_create(main_screen);
    lv_obj_add_style(btn3, &btn_style, 0);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0xE22E2F), 0);
    lv_obj_set_size(btn3, button_width, button_height);
    lv_obj_set_pos(btn3, button_x, button_y);
    lv_obj_add_event_cb(btn3, click_setting_button, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *label3 = lv_label_create(btn3);
    lv_obj_set_align(label3, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(label3, "设置");

    lv_obj_t *icn3 = lv_img_create(btn3);
    lv_obj_set_align(icn3, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(icn3, button_icn_x, 0);
    lv_img_set_src(icn3, &icn_Settings);
}

void settingScreenInit() {
    setting_screen = lv_obj_create(nullptr);

    // layout config
    lv_coord_t padding_x = 15;

    // menu
    lv_obj_t *menu = lv_menu_create(setting_screen);
    lv_obj_set_size(menu, lv_disp_get_hor_res(nullptr), lv_disp_get_ver_res(nullptr));
    lv_obj_center(menu);
    lv_menu_set_mode_root_back_btn(menu, LV_MENU_ROOT_BACK_BTN_ENABLED);
    lv_obj_set_style_bg_color(menu, lv_color_hex(0xEEEEEE), 0);
    lv_obj_add_event_cb(menu, click_menu, LV_EVENT_CLICKED, menu);

    lv_obj_t *cont, *section, *label, *img, *slider;


    // WiFi page
    wifi_page = lv_menu_page_create(menu, nullptr);
    lv_obj_set_style_pad_hor(wifi_page, padding_x, 0);
    lv_obj_set_style_text_font(wifi_page, &font_small, 0);

    lv_menu_separator_create(wifi_page);
    section = lv_menu_section_create(wifi_page);
    cont = lv_menu_cont_create(section);
    img = lv_img_create(cont);
    lv_img_set_src(img, &icn_WiFi);
    label = lv_label_create(cont);
    lv_label_set_text(label, "使用 WiFi");
    lv_obj_set_flex_grow(label, 1);
    wifi_switch = lv_switch_create(cont);
    lv_obj_add_event_cb(wifi_switch, change_wifi_switch, LV_EVENT_VALUE_CHANGED, nullptr);

    lv_menu_separator_create(wifi_page);
    wifi_state_section = lv_menu_section_create(wifi_page);
    cont = lv_menu_cont_create(wifi_state_section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "连接的网络");
    lv_obj_set_flex_grow(label, 1);
    wifi_connect_state_label = lv_label_create(cont);
    lv_label_set_text(wifi_connect_state_label, "");
    wifi_disconnect_button = lv_btn_create(cont);
    lv_obj_add_event_cb(wifi_disconnect_button, click_wifi_disconnect, LV_EVENT_CLICKED, nullptr);
    lv_obj_set_style_bg_color(wifi_disconnect_button, lv_color_hex(COLOR_BAD), 0);
    lv_obj_set_style_text_color(wifi_disconnect_button, lv_color_white(), 0);
    label = lv_label_create(wifi_disconnect_button);
    lv_label_set_text(label, "断开");
    lv_obj_set_style_pad_all(label, -7, 0);

    lv_menu_separator_create(wifi_page);
    cont = lv_menu_cont_create(wifi_page);
    wifi_list_label = lv_label_create(cont);
    lv_label_set_text(wifi_list_label, "可用的网络");
    lv_obj_set_flex_grow(wifi_list_label, 1);
    wifi_refresh_button = lv_btn_create(cont);
    lv_obj_add_event_cb(wifi_refresh_button, click_wifi_refresh, LV_EVENT_CLICKED, nullptr);
    lv_obj_set_style_bg_color(wifi_refresh_button, lv_color_hex(COLOR_NORMAL), 0);
    lv_obj_set_style_text_color(wifi_refresh_button, lv_color_white(), 0);
    label = lv_label_create(wifi_refresh_button);
    lv_label_set_text(label, "刷新");
    lv_obj_set_style_pad_all(label, -7, 0);


    // sound page
    lv_obj_t *sound_page = lv_menu_page_create(menu, nullptr);
    lv_obj_set_style_pad_hor(sound_page, padding_x, 0);
    lv_obj_set_style_text_font(sound_page, &font_small, 0);

    //section 1
    lv_menu_separator_create(sound_page);
    section = lv_menu_section_create(sound_page);

    cont = lv_menu_cont_create(section);
    img = lv_img_create(cont);
    lv_img_set_src(img, &icn_Mute);
    label = lv_label_create(cont);
    lv_label_set_text(label, "静音");
    lv_obj_set_flex_grow(label, 1);
    audio_mute_switch = lv_switch_create(cont);

    // section 2
    lv_menu_separator_create(sound_page);
    audio_volume_section = lv_menu_section_create(sound_page);

    cont = lv_menu_cont_create(audio_volume_section);
    img = lv_img_create(cont);
    lv_img_set_src(img, &icn_Volume);
    label = lv_label_create(cont);
    lv_label_set_text(label, "音量");
    lv_obj_set_flex_grow(label, 1);
    audio_volume_label = lv_label_create(cont);
    lv_label_set_text(audio_volume_label, "50");
    label = lv_label_create(cont);
    lv_label_set_text(label, " ");
    slider = lv_slider_create(cont);
    lv_obj_set_width(slider, 150);

    // section 3
    lv_menu_separator_create(sound_page);
    audio_music_section = lv_menu_section_create(sound_page);

    cont = lv_menu_cont_create(audio_music_section);
    img = lv_img_create(cont);
    lv_img_set_src(img, &icn_Music);
    label = lv_label_create(cont);
    lv_label_set_text(label, "音乐模式");
    lv_obj_set_flex_grow(label, 1);
    audio_music_switch = lv_switch_create(cont);


    // RGB page
    lv_obj_t *rgb_page = lv_menu_page_create(menu, nullptr);
    lv_obj_set_style_pad_hor(rgb_page, padding_x, 0);
    lv_obj_set_style_text_font(rgb_page, &font_small, 0);

    // section 1
    lv_menu_separator_create(rgb_page);
    cont = lv_menu_cont_create(rgb_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "顶部灯条");

    section = lv_menu_section_create(rgb_page);

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "开启");

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "效果");

    // section 2
    lv_menu_separator_create(rgb_page);
    cont = lv_menu_cont_create(rgb_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "侧壁灯条（上）");

    section = lv_menu_section_create(rgb_page);

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "开启");

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "效果");

    // section 3
    lv_menu_separator_create(rgb_page);
    cont = lv_menu_cont_create(rgb_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "侧壁灯条（下）");

    section = lv_menu_section_create(rgb_page);

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "开启");

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "效果");

    // section 4
    lv_menu_separator_create(rgb_page);
    cont = lv_menu_cont_create(rgb_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "灯塔");

    section = lv_menu_section_create(rgb_page);

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "开启");

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "效果");


    // root page
    lv_obj_t *root_page = lv_menu_page_create(menu, "Settings");
    lv_obj_set_style_text_font(root_page, &font_small, 0);
    lv_obj_set_style_pad_hor(root_page, padding_x, 0);

    // section 1
    cont = lv_menu_cont_create(root_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "通用");

    section = lv_menu_section_create(root_page);

    cont = lv_menu_cont_create(section);
    img = lv_img_create(cont);
    lv_img_set_src(img, &icn_WiFi);
    label = lv_label_create(cont);
    lv_label_set_text(label, "WiFi");
    lv_menu_set_load_page_event(menu, cont, wifi_page);

    cont = lv_menu_cont_create(section);
    img = lv_img_create(cont);
    lv_img_set_src(img, &icn_Sound);
    label = lv_label_create(cont);
    lv_label_set_text(label, "声音");
    lv_menu_set_load_page_event(menu, cont, sound_page);

    cont = lv_menu_cont_create(section);
    img = lv_img_create(cont);
    lv_img_set_src(img, &icn_RGB);
    label = lv_label_create(cont);
    lv_label_set_text(label, "灯效");
    lv_menu_set_load_page_event(menu, cont, rgb_page);

    // section 2
    cont = lv_menu_cont_create(root_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "其它");

    section = lv_menu_section_create(root_page);

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "系统升级");

    cont = lv_menu_cont_create(section);
    label = lv_label_create(cont);
    lv_label_set_text(label, "关于");

    lv_menu_set_sidebar_page(menu, root_page);
    lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 1), 0), LV_EVENT_CLICKED,
                  nullptr);

    UI_update_wifi_state();
}

void grabScreenInit() {
    grab_screen = lv_obj_create(nullptr);
    lv_obj_set_style_text_color(grab_screen, lv_color_white(), 0);
    lv_obj_set_style_text_font(grab_screen, &font_small, 0);


    lv_obj_t *img = lv_img_create(grab_screen);
    lv_img_set_src(img, &img_GrabPage);
    lv_obj_center(img);


    lv_coord_t pos_x = 27;
    lv_coord_t pos_y = 27;


    grab_label = lv_label_create(grab_screen);
    lv_label_set_text(grab_label, "");
    lv_obj_set_align(grab_label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_text_font(grab_label, &font_middle, 0);
    lv_obj_set_pos(grab_label, pos_x, pos_y);


    grab_back_button = lv_btn_create(grab_screen);
    lv_obj_set_style_bg_color(grab_back_button, lv_color_hex(COLOR_NORMAL), 0);
    lv_obj_set_align(grab_back_button, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(grab_back_button, -130, pos_y - 5);
    lv_obj_add_event_cb(grab_back_button, click_back, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *label = lv_label_create(grab_back_button);
    lv_label_set_text(label, "返回");


    grab_start_button = lv_btn_create(grab_screen);
    lv_obj_set_style_bg_color(grab_start_button, lv_color_hex(COLOR_GOOD), 0);
    lv_obj_set_align(grab_start_button, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(grab_start_button, -pos_x, pos_y - 5);
    lv_obj_add_event_cb(grab_start_button, click_grab_start, LV_EVENT_CLICKED, nullptr);
    lv_obj_add_state(grab_start_button, LV_STATE_DISABLED);


    label = lv_label_create(grab_start_button);
    lv_label_set_text(label, "开始！");


    grab_finish_button = lv_btn_create(grab_screen);
    lv_obj_set_style_bg_color(grab_finish_button, lv_color_hex(COLOR_GOOD), 0);
    lv_obj_set_align(grab_finish_button, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(grab_finish_button, -pos_x, pos_y - 5);
    lv_obj_add_event_cb(grab_finish_button, click_grab_finish, LV_EVENT_CLICKED, nullptr);
    lv_obj_add_flag(grab_finish_button, LV_OBJ_FLAG_HIDDEN);

    label = lv_label_create(grab_finish_button);
    lv_label_set_text(label, "完成");
}
