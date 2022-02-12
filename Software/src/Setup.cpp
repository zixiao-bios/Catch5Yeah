#include "Setup.h"

DynamicJsonDocument setup_doc(1024);

void init_setup() {
    File file = SPIFFS.open("/Setup.json");
    String json_str = file.readString();
    file.close();
    deserializeJson(setup_doc, json_str);
}
