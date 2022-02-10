#ifndef SOFTWARE_CLAW_H
#define SOFTWARE_CLAW_H

#include <ShiftRegister74HC595.h>
#include <esp_task_wdt.h>
#include "../Config.h"

#define RIGHT 0
#define LEFT 1
#define UP 2
#define DOWN 3
#define ALL 4

void claw_init();

void claw_reset_async();

void claw_grab_start();

void claw_grab_finish();

void claw_grab_timeout();

void turntable_set_rotate(bool rotate);

#endif //SOFTWARE_CLAW_H
