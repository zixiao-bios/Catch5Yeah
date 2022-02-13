#ifndef SOFTWARE_SETUP_H
#define SOFTWARE_SETUP_H

#include <LITTLEFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "RTC.h"

extern DynamicJsonDocument setup_doc;

void init_setup();

void write_setup();

int get_grab_num();

void record_grab_once();

#endif //SOFTWARE_SETUP_H
