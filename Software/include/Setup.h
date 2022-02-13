#ifndef SOFTWARE_SETUP_H
#define SOFTWARE_SETUP_H

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "RTC.h"

extern DynamicJsonDocument setup_doc;

void init_setup();

void write_setup();

void set_grab_available(int num);

#endif //SOFTWARE_SETUP_H
