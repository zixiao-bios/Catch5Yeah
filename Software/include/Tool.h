#ifndef SOFTWARE_TOOL_H
#define SOFTWARE_TOOL_H

#include <Arduino.h>
#include <FS.h>

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

#endif //SOFTWARE_TOOL_H
