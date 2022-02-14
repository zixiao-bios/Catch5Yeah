#ifndef SOFTWARE_AUDIO_H
#define SOFTWARE_AUDIO_H

#include <SPIFFS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include <AudioFileSourceLittleFS.h>
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"
#include "AudioFileSourceHTTPStream.h"
#include "../Config.h"

enum playMode {
    Once, Loop
};

class Audio {
public:
    bool play(const String& filename);

    void setPlayMode(playMode playMode);

    void stop();

private:
    String fileName;
    AudioFileSourceLittleFS *file = nullptr;
    AudioGeneratorMP3 *mp3 = nullptr;
    AudioOutputI2S *out = nullptr;

    bool stop_flag = false;
    playMode mode = Once;

    void initPlay();
    void releaseInit();
    static void playHandle(void *pv);
};


#endif //SOFTWARE_AUDIO_H
