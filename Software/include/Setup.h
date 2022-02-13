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

// get remaining grab num today
int get_grab_remain();

void record_grab_once();

#endif //SOFTWARE_SETUP_H
