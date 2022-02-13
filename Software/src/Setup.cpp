#include "Setup.h"

DynamicJsonDocument setup_doc(1024);

void printjson() {
    serializeJson(setup_doc, Serial);
}

void init_setup() {
    File f = LITTLEFS.open("/Setup.json");
    deserializeJson(setup_doc, f);
    f.close();
}

void write_setup() {
    File f = LITTLEFS.open("/Setup.json", "w");
    serializeJson(setup_doc, f);
    f.close();
}

int get_grab_num() {
    if (setup_doc["grab"]["time"] < 0 or !is_same_day(get_timestamp(), setup_doc["grab"]["time"])) {
        // today hasn't grab yet
        return 0;
    } else {
        return setup_doc["grab"]["num"];
    }
}

void record_grab_once() {
    if (setup_doc["grab"]["time"] < 0 or !is_same_day(setup_doc["grab"]["time"], get_timestamp())) {
        // today first grab
        setup_doc["grab"]["time"] = get_timestamp();
        setup_doc["grab"]["num"] = 1;
    } else {
        setup_doc["grab"]["time"] = get_timestamp();
        setup_doc["grab"]["num"] = (int)setup_doc["grab"]["num"] + 1;
    }
    write_setup();
}
