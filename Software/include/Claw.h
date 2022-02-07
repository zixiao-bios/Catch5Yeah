#ifndef SOFTWARE_CLAW_H
#define SOFTWARE_CLAW_H

#include <ShiftRegister74HC595.h>
#include "../Config.h"

void claw_init();

void claw_set_controllable(bool flag);

bool claw_at_end(int dir);

void move(int dir);

void move_to_end(int dir);

void stop_x();

void stop_y();

#endif //SOFTWARE_CLAW_H
