#ifndef SOFTWARE_SETUP_H
#define SOFTWARE_SETUP_H

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>

extern DynamicJsonDocument setup_doc;

void init_setup();

void write_setup();

#endif //SOFTWARE_SETUP_H
