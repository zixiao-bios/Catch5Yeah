#include "Claw.h"

ShiftRegister74HC595<1> *sr;
bool controllable = false;
bool moving_to_end = false;

void move(int dir) {
    // stop move if reach the end
    if (claw_at_end(dir)) {
        if (dir == CLAW_UP or dir == CLAW_DOWN) {
            stop_y();
        } else {
            stop_x();
        }
        return;
    }

    // stop move x if claw not at the top
    if (!claw_at_end(CLAW_UP) and (dir == CLAW_LEFT or dir == CLAW_RIGHT)) {
        stop_x();
        return;
    }

    switch (dir) {
        case CLAW_LEFT:
            sr->setNoUpdate(SR_PIN_M1A, LOW);
            sr->setNoUpdate(SR_PIN_M1B, HIGH);
            break;
        case CLAW_RIGHT:
            sr->setNoUpdate(SR_PIN_M1A, HIGH);
            sr->setNoUpdate(SR_PIN_M1B, LOW);
            break;
        case CLAW_UP:
            sr->setNoUpdate(SR_PIN_M2A, HIGH);
            sr->setNoUpdate(SR_PIN_M2B, LOW);
            break;
        case CLAW_DOWN:
            sr->setNoUpdate(SR_PIN_M2A, LOW);
            sr->setNoUpdate(SR_PIN_M2B, HIGH);
            break;
        default:
            break;
    }
    sr->updateRegisters();
}

[[noreturn]] void control_task(void *pv) {
    // move claw to left end
    move_to_end(CLAW_UP);
    move_to_end(CLAW_LEFT);

    TickType_t lastWakeTime;

    while (true) {
        if (controllable && !moving_to_end) {
            if (!digitalRead(PIN_KEY_RIGHT)) {
                move(CLAW_RIGHT);
            } else if (!digitalRead(PIN_KEY_LEFT)) {
                move(CLAW_LEFT);
            } else {
                stop_x();
            }
        }

        vTaskDelayUntil(&lastWakeTime, 10 / portTICK_PERIOD_MS);
    }
}

void claw_init() {
    pinMode(PIN_S_RIGHT, INPUT);
    pinMode(PIN_S_LEFT, INPUT);
    pinMode(PIN_S_UP, INPUT);

    sr = new ShiftRegister74HC595<1>(PIN_SR_SI, PIN_SR_SCK, PIN_SR_RCK);
    sr->setAllLow();
    sr->setNoUpdate(SR_PIN_SCREEN, HIGH);
    sr->updateRegisters();

    xTaskCreatePinnedToCore(control_task, "ClawControl", 1024, nullptr, 2, nullptr, 0);
}

void claw_set_controllable(bool flag) {
    controllable = flag;

    if (!moving_to_end) {
        stop_x();
        stop_y();
    }
}

bool claw_at_end(int dir) {
    if (dir == CLAW_UP) {
         return !digitalRead(PIN_S_UP);
    } else if (dir == CLAW_RIGHT) {
        return !digitalRead(PIN_S_RIGHT);
    } else if (dir == CLAW_LEFT) {
        return !digitalRead(PIN_S_LEFT);
    }
}

void move_to_end(int dir) {
    if (!claw_at_end(CLAW_UP) and (dir == CLAW_LEFT or dir == CLAW_RIGHT)) {
        stop_x();
        return;
    }

    moving_to_end = true;

    TickType_t lastWakeTime;
    while (!claw_at_end(dir)) {
        move(dir);
        vTaskDelayUntil(&lastWakeTime, 10 / portTICK_PERIOD_MS);
    }

    if (dir == CLAW_LEFT or dir == CLAW_RIGHT) {
        stop_x();
    } else {
        stop_y();
    }

    moving_to_end = false;
}

void stop_x() {
    sr->setNoUpdate(SR_PIN_M1A, LOW);
    sr->setNoUpdate(SR_PIN_M1B, LOW);
    sr->updateRegisters();
}

void stop_y() {
    sr->setNoUpdate(SR_PIN_M2A, LOW);
    sr->setNoUpdate(SR_PIN_M2B, LOW);
    sr->updateRegisters();
}
