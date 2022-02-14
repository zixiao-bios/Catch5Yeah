#include "Audio.h"

bool Audio::play(const char *filename) {
    this->fileName = filename;
    if (xTaskCreatePinnedToCore(&Audio::playHandle, "playHandle", 4096, this, 2,
                    &this->playTaskHandle, 1) == pdPASS) {
        this->playing = true;
        return true;
    } else {
        return false;
    }
}

void Audio::setPlayMode(playMode playMode) {
    this->mode = playMode;
}

[[noreturn]] void Audio::playHandle(void *pv) {
    auto *self = (Audio *) pv;
    while (true) {
        do {
            self->initPlay();
            // playing music
            self->mp3->begin(self->file, self->out);
            TickType_t lastWakeTime = xTaskGetTickCount();
            while (self->mp3->isRunning()) {
                if (!self->mp3->loop()) {
                    self->mp3->stop();
                }
                vTaskDelayUntil(&lastWakeTime, 20 / portTICK_PERIOD_MS);
            }
            delay(20);
        } while (self->mode == Loop);

        // playback finish, delete task
        self->playing = false;
        self->destroyPlayHandle();
    }
}

void Audio::stop() {
    // stop playing
    if (this->playing && this->mp3 != nullptr) {
        this->mp3->stop();
        this->playing = false;
    }

    // destroy play task
    if (this->playTaskHandle != nullptr) {
        vTaskDelete(this->playTaskHandle);
        this->playTaskHandle = nullptr;
    }

    // release audio init
    this->releaseInit();
}

void Audio::initPlay() {
    this->releaseInit();

    file = new AudioFileSourceLittleFS(this->fileName);
    mp3 = new AudioGeneratorMP3();
    out = new AudioOutputI2S();
    out->SetPinout(PIN_I2S_BLCK, PIN_I2S_LRC, PIN_I2S_DIN);
}

void Audio::destroyPlayHandle() {
    if (this->playTaskHandle != nullptr) {
        vTaskDelete(this->playTaskHandle);
        this->playTaskHandle = nullptr;
    }
}

void Audio::releaseInit() {
    delete this->file;
    delete this->mp3;
    delete this->out;
}
