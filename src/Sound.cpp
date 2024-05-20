#define MINIAUDIO_IMPLEMENTATION // Attention à ne pas oublier cette ligne, sinon miniaudio n'est pas correctement inclus
#include <Sound.hpp>

Sound::Sound(){
    // Initialiser le moteur de miniaudio
    if (ma_engine_init(NULL, &(this->engine)) != MA_SUCCESS) {
        std::cerr << "Erreur lors de l'initialisation du moteur miniaudio" << std::endl;
    }

    this->isEffectPlayed = false;
    this->isBackgroundSoundPlayed = false;
    this->seagull_1 = new ma_sound;
    this->seagull_2 = new ma_sound;
    this->plouf = new ma_sound;
    this->backgroundSound = new ma_sound;
    ma_sound_init_from_file(&(this->engine), "../sound/soundEffect/seagull_1.mp3", 0, NULL, NULL, this->seagull_1);
    ma_sound_init_from_file(&(this->engine), "../sound/soundEffect/seagull_2.mp3", 0, NULL, NULL, this->seagull_2);
    ma_sound_init_from_file(&(this->engine), "../sound/soundEffect/plouf.mp3", 0, NULL, NULL, this->plouf);
    ma_sound_init_from_file(&(this->engine), "../sound/backgroundMusic/wave_background.mp3", 0, NULL, NULL, this->backgroundSound);
}

Sound::~Sound(){
    ma_sound_uninit(this->seagull_1);
    delete this->seagull_1;
    ma_sound_uninit(this->seagull_2);
    delete this->seagull_2;
    ma_sound_uninit(this->plouf);
    delete this->plouf;

    ma_engine_uninit(&(this->engine));
}

void Sound::playSeagull1(){
    this->isEffectPlayed = true;
    ma_sound_start(this->seagull_1);
}

void Sound::playSeagull2(){
    this->isEffectPlayed = true;
    ma_sound_start(this->seagull_2);
}

void Sound::playPlouf(){
	ma_sound_start(this->plouf);
}

void Sound::playBackgroundSound(){
    this->isBackgroundSoundPlayed = true;
    ma_sound_start(this->backgroundSound);
}

ma_sound* Sound::getSeagull1(){
    return this->seagull_1;
}

ma_sound* Sound::getSeagull2(){
    return this->seagull_2;
}

ma_sound* Sound::getPlouf(){
    return this->plouf;
}

ma_sound* Sound::getBackgroundSound(){
    return this->backgroundSound;
}

bool Sound::getEffectPlayed(){
    return this->isEffectPlayed;
}

bool Sound::getBackgroundPlayed(){
    return this->isBackgroundSoundPlayed;
}

void Sound::setIsEffectPlayed(bool value){
    this->isEffectPlayed = value;
}

void Sound::setIsBackgroundPlayed(bool value){
    this->isBackgroundSoundPlayed = value;
}




