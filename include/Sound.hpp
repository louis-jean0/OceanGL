#pragma once

#include "miniaudio.h"

#include <Headers.hpp>

class Sound{
    private :
        ma_engine engine;
        bool isEffectPlayed;
        bool isBackgroundSoundPlayed;
        ma_sound* seagull_1;
        ma_sound* seagull_2;
        ma_sound* backgroundSound;
        ma_sound* plouf;
    public : 
        Sound();
        ~Sound();
        void playSeagull1();
        void playSeagull2();
        void playPlouf();
        void playBackgroundSound();
        bool getEffectPlayed();
        bool getBackgroundPlayed();
        void setIsEffectPlayed(bool value);
        void setIsBackgroundPlayed(bool value);
        ma_sound* getSeagull1();
        ma_sound* getSeagull2();
        ma_sound* getPlouf();
        ma_sound* getBackgroundSound();
};
