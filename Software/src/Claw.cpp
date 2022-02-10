#include "Claw.h"

ShiftRegister74HC595<1> *sr;
bool at_top = false;

// stay_top_task is running
bool stay_top_task = false;
bool grab_exit = false;
bool grab_time_out = false;
bool grab_done = false;

SemaphoreHandle_t claw_x_mutex, claw_y_mutex, sr_mutex;

void mag_set(bool mag) {
    xSemaphoreTake(sr_mutex, portMAX_DELAY);

    sr->setNoUpdate(SR_PIN_MAG, mag);
    sr->updateRegisters();

    xSemaphoreGive(sr_mutex);
}

bool claw_at_end(int dir) {
    if (dir == UP) {
        return !digitalRead(PIN_S_UP);
    } else if (dir == RIGHT) {
        return !digitalRead(PIN_S_RIGHT);
    } else if (dir == LEFT) {
        return !digitalRead(PIN_S_LEFT);
    } else if (dir == DOWN) {
        return false;
    }
}

bool dir_x(int dir) {
    return (dir == LEFT or dir == RIGHT or dir == ALL);
}

bool dir_y(int dir) {
    return (dir == UP or dir == DOWN or dir == ALL);
}

void stop(int dir) {
    xSemaphoreTake(sr_mutex, portMAX_DELAY);
    if (dir_x(dir)) {
        sr->setNoUpdate(SR_PIN_M1A, LOW);
        sr->setNoUpdate(SR_PIN_M1B, LOW);
    }
    if (dir_y(dir)) {
        sr->setNoUpdate(SR_PIN_M2A, LOW);
        sr->setNoUpdate(SR_PIN_M2B, LOW);
    }
    sr->updateRegisters();
    xSemaphoreGive(sr_mutex);
}

bool move(int dir) {
    // stop move if reach the end
    if (claw_at_end(dir)) {
        stop(dir);
        return false;
    }

    // stop move x if claw not at the top
    if (!at_top and (dir_x(dir))) {
        stop(dir);
        return false;
    }

    xSemaphoreTake(sr_mutex, portMAX_DELAY);
    switch (dir) {
        case LEFT:
            sr->setNoUpdate(SR_PIN_M1A, LOW);
            sr->setNoUpdate(SR_PIN_M1B, HIGH);
            break;
        case RIGHT:
            sr->setNoUpdate(SR_PIN_M1A, HIGH);
            sr->setNoUpdate(SR_PIN_M1B, LOW);
            break;
        case UP:
            sr->setNoUpdate(SR_PIN_M2A, HIGH);
            sr->setNoUpdate(SR_PIN_M2B, LOW);
            break;
        case DOWN:
            sr->setNoUpdate(SR_PIN_M2A, LOW);
            sr->setNoUpdate(SR_PIN_M2B, HIGH);
            break;
        default:
            break;
    }
    sr->updateRegisters();
    xSemaphoreGive(sr_mutex);
    return true;
}

void move_to_end(int dir) {
    if (dir_y(dir)) {
        return;
    }

    TickType_t lastWakeTime;
    while (!claw_at_end(dir)) {
        move(dir);
        vTaskDelayUntil(&lastWakeTime, 10 / portTICK_PERIOD_MS);
    }
    stop(dir);
}

[[noreturn]] void claw_stay_top_task(void *pv) {
    xSemaphoreTake(claw_y_mutex, portMAX_DELAY);

    TickType_t lastWakeTime;
    while (stay_top_task) {
        if (claw_at_end(UP)) {
            at_top = true;
        }
        move(UP);
        vTaskDelayUntil(&lastWakeTime, 10 / portTICK_PERIOD_MS);
    }
    stop(UP);
    at_top = false;

    xSemaphoreGive(claw_y_mutex);
    vTaskDelete(nullptr);
}

void claw_stay_top_async() {
    if (!stay_top_task) {
        stay_top_task = true;
        xTaskCreatePinnedToCore(claw_stay_top_task, "StayTop", 2048, nullptr, 2, nullptr, 0);
    }
}

void claw_stay_top_cancel() {
    stay_top_task = false;
}

[[noreturn]] void claw_grab_task(void *pv) {
    esp_task_wdt_init(120, true);

    xSemaphoreTake(claw_x_mutex, portMAX_DELAY);
    turntable_set_rotate(true);
    grab_exit = false;
    grab_time_out = false;
    grab_done =false;

    claw_stay_top_async();
    move_to_end(LEFT);

    TickType_t lastWakeTime;
    while (digitalRead(PIN_BTN) and !grab_time_out) {
        if (!digitalRead(PIN_KEY_RIGHT)) {
            move(RIGHT);
        } else if (!digitalRead(PIN_KEY_LEFT)) {
            move(LEFT);
        } else {
            stop(LEFT);
        }

        vTaskDelayUntil(&lastWakeTime, 10 / portTICK_PERIOD_MS);
    }

    // push button or timeout
    stop(LEFT);
    turntable_set_rotate(false);
    claw_stay_top_cancel();

    // move down
    bool btn_release = false;
    xSemaphoreTake(claw_y_mutex, portMAX_DELAY);
    for (int i = 0; i < 300; ++i) {
        if (digitalRead(PIN_BTN)) {
            btn_release = true;
        }
        if (!digitalRead(PIN_BTN) && btn_release) {
            break;
        }
        if (i > 250) {
            move(DOWN);
        }
        vTaskDelayUntil(&lastWakeTime, 20 / portTICK_PERIOD_MS);
    }
    stop(DOWN);
    xSemaphoreGive(claw_y_mutex);

    // push button twice, grab done
    grab_done = true;
    mag_set(true);
    claw_stay_top_async();
    move_to_end(LEFT);
    delay(1000);
    claw_stay_top_cancel();

    // move down
    xSemaphoreTake(claw_y_mutex, portMAX_DELAY);
    delay(1000);
    xSemaphoreGive(claw_y_mutex);

    mag_set(false);
    claw_stay_top_async();
    while (!grab_exit) {
        delay(100);
    }
    claw_stay_top_cancel();

    xSemaphoreGive(claw_x_mutex);
    esp_task_wdt_init(4, true);
    vTaskDelete(nullptr);
}

[[noreturn]] void claw_reset_task(void *pv) {
    xSemaphoreTake(claw_x_mutex, portMAX_DELAY);

    claw_stay_top_async();
    move_to_end(LEFT);
    claw_stay_top_cancel();

    xSemaphoreGive(claw_x_mutex);

    vTaskDelete(nullptr);
}

void claw_init() {
    claw_x_mutex = xSemaphoreCreateMutex();
    claw_y_mutex = xSemaphoreCreateMutex();
    sr_mutex = xSemaphoreCreateMutex();

    pinMode(PIN_KEY_DOWN, INPUT);
    pinMode(PIN_KEY_UP, INPUT);
    pinMode(PIN_KEY_RIGHT, INPUT);
    pinMode(PIN_KEY_LEFT, INPUT);
    pinMode(PIN_BTN, INPUT);
    pinMode(PIN_S_RIGHT, INPUT);
    pinMode(PIN_S_LEFT, INPUT);
    pinMode(PIN_S_UP, INPUT);

    xSemaphoreTake(sr_mutex, portMAX_DELAY);
    sr = new ShiftRegister74HC595<1>(PIN_SR_SI, PIN_SR_SCK, PIN_SR_RCK);
    sr->setAllLow();
    sr->setNoUpdate(SR_PIN_SCREEN, HIGH);
    sr->updateRegisters();
    xSemaphoreGive(sr_mutex);
}

void claw_reset_async() {
    xTaskCreatePinnedToCore(claw_reset_task, "ClawReset", 2048, nullptr, 2, nullptr, 0);
}

void claw_grab_start() {
    xTaskCreatePinnedToCore(claw_grab_task, "ClawGrab", 2048, nullptr, 2, nullptr, 0);
}

void claw_grab_exit() {
    grab_exit = true;
}

void claw_grab_timeout() {
    grab_time_out = true;
}

bool claw_grab_is_done() {
    return grab_done;
}

void turntable_set_rotate(bool rotate) {
    xSemaphoreTake(sr_mutex, portMAX_DELAY);

    sr->setNoUpdate(SR_PIN_M3, rotate);
    sr->updateRegisters();

    xSemaphoreGive(sr_mutex);
}
