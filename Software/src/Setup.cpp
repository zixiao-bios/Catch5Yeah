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
