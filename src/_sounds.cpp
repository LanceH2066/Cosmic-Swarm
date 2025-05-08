#include "_sounds.h"
using namespace irrklang;

_sounds::_sounds()
{
    if (!soundEngine)
    {
        std::cout << "ERROR: Sound Engine Could Not Start" << std::endl;
        return;
    }

    // Preload sound sources
    musicSource            = soundEngine->addSoundSourceFromFile("sounds/music.wav");
    damagePlayerSource     = soundEngine->addSoundSourceFromFile("sounds/damage1.wav");
    damageEnemySource      = soundEngine->addSoundSourceFromFile("sounds/damage2.wav");
    rocketFireSource       = soundEngine->addSoundSourceFromFile("sounds/rocketFire.wav");
    enemyDeathSource       = soundEngine->addSoundSourceFromFile("sounds/explosion1.wav");
    rocketExplosionSource  = soundEngine->addSoundSourceFromFile("sounds/explosion3.wav");
    laserCannonSource      = soundEngine->addSoundSourceFromFile("sounds/laserCannon.wav");
    flakSource             = soundEngine->addSoundSourceFromFile("sounds/flak.wav");

    musicSource->setDefaultVolume(0.05f);
    damagePlayerSource->setDefaultVolume(0.05f);
    damageEnemySource->setDefaultVolume(0.02f);
    rocketFireSource->setDefaultVolume(0.1f);
    enemyDeathSource->setDefaultVolume(0.05f);
    rocketExplosionSource->setDefaultVolume(0.2f);
    laserCannonSource->setDefaultVolume(0.03f);
    flakSource->setDefaultVolume(0.02f);

}

_sounds::~_sounds()
{
    stopMusic();
    soundEngine->drop();
}

void _sounds::play(ISoundSource* source)
{
    if (source)
    {
        ISound* s = soundEngine->play2D(source, false, false, true);
        if (s) activeSounds.push_back(s);
    }
}

void _sounds::playMusic()
{
    if (currentMusic)
    {
        currentMusic->stop();
        currentMusic->drop();
    }
    currentMusic = soundEngine->play2D(musicSource, true, false, true);
}

void _sounds::stopMusic()
{
    if (currentMusic)
    {
        currentMusic->stop();
        currentMusic->drop();
        currentMusic = nullptr;
    }
}

void _sounds::playEnemyDamage()
{
    if (activeEnemyDamageSounds < maxEnemyDamageSounds)
    {
        ISound* s = soundEngine->play2D(damageEnemySource, false, false, true);
        if (s)
        {
            activeSounds.push_back(s);
            activeEnemyDamageSounds++;
        }
    }
}

void _sounds::playEnemyDeath()
{
    if (activeEnemyDeathSounds < maxEnemyDeathSounds)
    {
        ISound* s = soundEngine->play2D(enemyDeathSource, false, false, true);
        if (s)
        {
            activeSounds.push_back(s);
            activeEnemyDeathSounds++;
        }
    }
}

// Remove finished sounds from the active sounds list
void _sounds::cleanupSounds()
{
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
            [this](ISound* sound)
            {
                if (!sound || sound->isFinished())
                {
                    if (sound)
                    {
                        // Decrement counters by comparing sources
                        if (sound->getSoundSource() == damageEnemySource) activeEnemyDamageSounds--;
                        else if (sound->getSoundSource() == enemyDeathSource) activeEnemyDeathSounds--;

                        sound->drop();
                    }
                    return true;
                }
                return false;
            }),
        activeSounds.end());
}
