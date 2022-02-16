#ifndef SOFTWARE_MUSIC_H
#define SOFTWARE_MUSIC_H

#include <LITTLEFS.h>
#include <FS.h>
#include "Setup.h"

void music_init();

void play_one_loop();

void play_random_loop();

void play_stop();

bool is_playing();

void music_set_mute(bool m);

#endif //SOFTWARE_MUSIC_H
