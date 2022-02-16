#include "Audio.h"

SemaphoreHandle_t Audio::playMutex = xSemaphoreCreateMutex();

bool Audio::play_async(const String &filename) {
    this->fileName = filename;
    if (xTaskCreatePinnedToCore(&Audio::playHandle, "playHandle", 4096, this, 2,
                                nullptr, 1) == pdPASS) {
        return true;
    } else {
        return false;
    }
}

void Audio::setPlayMode(playMode playMode) {
    this->mode = playMode;
}

void Audio::playHandle(void *pv) {
    xSemaphoreTake(Audio::playMutex, portMAX_DELAY);

    auto *self = (Audio *) pv;
    self->stop_flag = false;
    do {
        self->initPlay();

        // playing music
        self->mp3->begin(self->file, self->out);
        TickType_t lastWakeTime = xTaskGetTickCount();
        while (self->mp3->isRunning()) {
            if (!self->mp3->loop() or self->stop_flag) {
                self->mp3->stop();
            }
            vTaskDelayUntil(&lastWakeTime, 10 / portTICK_PERIOD_MS);
        }
        delay(10);
    } while (self->mode == Loop and !self->stop_flag);

    xSemaphoreGive(Audio::playMutex);

    // playback finish, delete task
    vTaskDelete(nullptr);
}

void Audio::stop() {
    // stop playing
    this->stop_flag = true;
}

void Audio::initPlay() {
    this->releaseInit();

    file = new AudioFileSourceLittleFS(this->fileName.c_str());
    mp3 = new AudioGeneratorMP3();
    out = new AudioOutputI2S();
    out->SetPinout(PIN_I2S_BLCK, PIN_I2S_LRC, PIN_I2S_DIN);
    out->SetGain(this->volume);
}

void Audio::releaseInit() {
    delete this->file;
    delete this->mp3;
    delete this->out;
    this->file = nullptr;
    this->mp3 = nullptr;
    this->out = nullptr;
}

void Audio::setVolume(float v) {
    this->volume = v;
    if (out) {
        out->SetGain(v);
    }
}

void Audio::play(const String &filename) {
    xSemaphoreTake(Audio::playMutex, portMAX_DELAY);

    this->stop_flag = false;
    do {
        this->initPlay();

        // playing music
        this->mp3->begin(this->file, this->out);
        TickType_t lastWakeTime = xTaskGetTickCount();
        while (this->mp3->isRunning()) {
            if (!this->mp3->loop() or this->stop_flag) {
                this->mp3->stop();
            }
            vTaskDelayUntil(&lastWakeTime, 10 / portTICK_PERIOD_MS);
        }
        delay(10);
    } while (this->mode == Loop and !this->stop_flag);

    xSemaphoreGive(Audio::playMutex);
}
