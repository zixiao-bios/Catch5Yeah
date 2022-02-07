#include "Claw.h"

ShiftRegister74HC595<1> *sr;
bool move_flag = false;

[[noreturn]] void control_task(void *pv) {
    TickType_t lastWakeTime;

    while (true) {
        if (!digitalRead(PIN_KEY_RIGHT)) {
            move(CLAW_RIGHT);
        } else if (!digitalRead(PIN_KEY_LEFT)) {
            move(CLAW_LEFT);
        } else {
            stop_x();
        }

        vTaskDelayUntil(&lastWakeTime, 10 / portTICK_PERIOD_MS);
    }
}

void claw_init() {
    sr = new ShiftRegister74HC595<1>(PIN_SR_SI, PIN_SR_SCK, PIN_SR_RCK);
    sr->setAllLow();
    sr->setNoUpdate(SR_PIN_SCREEN, HIGH);
    sr->updateRegisters();

    xTaskCreatePinnedToCore(control_task, "ClawControl", 1024, nullptr, 2, nullptr, 0);
}

void claw_set_movable(bool flag) {
    move_flag = flag;
}

void move(int direction) {
    if (!move_flag) {
        return;
    }

    switch (direction) {
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
