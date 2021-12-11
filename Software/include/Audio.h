//
// Created by zixiao_bios on 2021/12/10.
//

#ifndef SOFTWARE_AUDIO_H
#define SOFTWARE_AUDIO_H

#include <SPIFFS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"
#include "AudioFileSourceHTTPStream.h"

enum playMode {
    Once, Loop
};

class Audio {
public:
    bool play(const char *filename);

    void setPlayMode(playMode playMode);

    void stop();

private:
    const char *fileName = nullptr;
    AudioFileSourceSPIFFS *file = nullptr;
    AudioGeneratorMP3 *mp3 = nullptr;
    AudioOutputI2S *out = nullptr;
    TaskHandle_t playTaskHandle = nullptr;

    bool playing = false;
    playMode mode = Once;

    void initPlay();
    void releaseInit();
    void destroyPlayHandle();
    [[noreturn]] static void playHandle(void *pv);
};


#endif //SOFTWARE_AUDIO_H
