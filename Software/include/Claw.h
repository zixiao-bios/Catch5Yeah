#ifndef SOFTWARE_CLAW_H
#define SOFTWARE_CLAW_H

#include <ShiftRegister74HC595.h>
#include "../Config.h"

void claw_init();

void claw_set_movable(bool flag);

void move(int direction);

void move_to_end(int direction);

void stop_x();

void stop_y();

#endif //SOFTWARE_CLAW_H
