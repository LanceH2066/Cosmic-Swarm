#ifndef _SOUNDS_H
#define _SOUNDS_H

#include <SNDS/irrKlang.h>
#include <_common.h>
#include <vector>

using namespace irrklang;

class _sounds
{
public:
    irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();

    _sounds();
    ~_sounds();

    void playMusic();
    void stopMusic();
    void playThrusterSound();
    void stopThrusterSound();
    void playShootSound();

private:
    ISound* currentMusic = nullptr;   // Background music
    ISound* thrusterSound = nullptr;  // Looping thruster sound

    ISoundSource* thrusterSoundSource = nullptr;
    ISoundSource* shootSoundSource = nullptr;
    ISoundSource* musicSource = nullptr;

    vector<ISound*> activeSounds;  // Store currently playing sounds
    void cleanupSounds();  // Remove finished sounds
};

#endif // _SOUNDS_H
