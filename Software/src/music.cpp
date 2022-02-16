#include <Audio.h>
#include "music.h"

int music_num = 0;
String *music_names;
Audio *audio;
bool playing = false;
bool mute;

void music_init() {
    File dir = LITTLEFS.open("/music");
    File f = dir.openNextFile();
    while (f) {
        music_num++;
        f.close();
        f = dir.openNextFile();
    }
    dir.close();

    music_names = new String[music_num];
    dir = LITTLEFS.open("/music");
    f = dir.openNextFile();
    for (int i = 0; i < music_num; ++i) {
        music_names[i] = f.name();
        f.close();
        f = dir.openNextFile();
    }
    dir.close();

    audio = new Audio();

    mute = setup_doc["audio"]["mute"];
}

void play_one_loop() {
    if (mute) {
        return;
    }

    if (is_playing()) {
        play_stop();
    }

    audio->play(music_names[random(music_num)]);
    audio->setPlayMode(Loop);
    playing = true;
}

void play_random_loop_task(void *pv) {

}

void play_random_loop() {
    if (mute) {
        return;
    }

    if (is_playing()) {
        play_stop();
    }
}

void play_stop() {
    audio->stop();
    playing = false;
}

bool is_playing() {
    return playing;
}

void music_set_mute(bool m) {
    mute = m;
    if (m && playing) {
        play_stop();
    }
}
