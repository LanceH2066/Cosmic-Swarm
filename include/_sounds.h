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
    void play(ISoundSource* source);
    void playEnemyDamage();
    void playEnemyDeath();
    int activeEnemyDamageSounds = 0;
    int activeEnemyDeathSounds = 0;
    const int maxEnemyDamageSounds = 4;  // adjust as needed
    const int maxEnemyDeathSounds = 3;

    ISound* currentMusic = nullptr;
    ISoundSource* musicSource = nullptr;
    ISoundSource* damagePlayerSource;
    ISoundSource* damageEnemySource;
    ISoundSource* rocketFireSource;
    ISoundSource* enemyDeathSource;
    ISoundSource* rocketExplosionSource;
    ISoundSource* laserCannonSource;
    ISoundSource* flakSource;
    vector<ISound*> activeSounds;  // Store currently playing sounds
    void cleanupSounds();  // Remove finished sounds
private:


};

#endif // _SOUNDS_H
