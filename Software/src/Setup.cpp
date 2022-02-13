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
