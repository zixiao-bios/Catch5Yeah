#include "Setup.h"

DynamicJsonDocument setup_doc(1024);

void init_setup() {
    File f = SPIFFS.open("/Setup.json");
    deserializeJson(setup_doc, f);
    f.close();
}

void write_setup() {
    File f = SPIFFS.open("/Setup.json", "w");
    serializeJson(setup_doc, f);
    f.close();
}

void set_grab_available(int num) {
    setup_doc["grab"]["available_time"] = get_timestamp();
    setup_doc["grab"]["available_num"] = num;
    write_setup();
}

int get_grab_remain() {
    uint32_t available_time = setup_doc["grab"]["available_time"];
    uint32_t grab_time = setup_doc["grab"]["grab_time"];
    int available_num = setup_doc["grab"]["available_num"];
    int grab_num = setup_doc["grab"]["grab_num"];

    if (available_time < 0 or !is_same_day(available_time, get_timestamp())) {
        // hasn't updated today's grab time
        return -1;
    }
    if (grab_time < 0 or !is_same_day(available_time, grab_time)) {
        // today hasn't grab yet
        return available_num;
    } else {
        return available_num - grab_num;
    }
}
