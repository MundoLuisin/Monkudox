#pragma once

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#include <string>
#include <vector>

class AudioManager {
public:
    ma_engine engine;

    bool Init() {
        ma_engine_config config = ma_engine_config_init();
        config.listenerCount = 1;
        return ma_engine_init(&config, &engine) == MA_SUCCESS;
    }

    void PlaySound(const char* filename, float volume = 1.0f, bool loop = false) 
    {
        ma_sound* sound = new ma_sound(); 
        if (ma_sound_init_from_file(&engine, filename, 0, NULL, NULL, sound) == MA_SUCCESS) {
            ma_sound_set_volume(sound, volume);
            if (loop) {
                ma_sound_set_looping(sound, true);  
            }
            ma_sound_start(sound);
        }
        else {
            delete sound;
            printf("ERROR cargando sonido: %s\n", filename);
        }
    }

  
    ma_sound* LoadSound(const char* filename, bool loop = false, float volume = 1.0f) {
        ma_sound* sound = new ma_sound();
        if (ma_sound_init_from_file(&engine, filename, 0, NULL, NULL, sound) == MA_SUCCESS) {
            ma_sound_set_volume(sound, volume);
            ma_sound_set_looping(sound, loop);
            return sound;
        }
        delete sound;
        return nullptr;
    }

    void Shutdown() {
        ma_engine_uninit(&engine);
    }
};