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
    thrusterSoundSource = soundEngine->addSoundSourceFromFile("sounds/engineSound.mp3");
    shootSoundSource = soundEngine->addSoundSourceFromFile("sounds/blast.mp3");
    musicSource = soundEngine->addSoundSourceFromFile("sounds/music.mp3");

    // Set sounds to be played from memory for performance
    thrusterSoundSource->setDefaultVolume(0.1f);
    shootSoundSource->setDefaultVolume(0.01f);
    musicSource->setDefaultVolume(0.05f);
}

_sounds::~_sounds()
{
    stopMusic();
    soundEngine->drop();
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

void _sounds::playThrusterSound()
{
    if (!thrusterSound || thrusterSound->isFinished())
    {
        thrusterSound = soundEngine->play2D(thrusterSoundSource, true, false, true);
    }
}

void _sounds::stopThrusterSound()
{
    if (thrusterSound)
    {
        thrusterSound->stop();
        thrusterSound->drop();
        thrusterSound = nullptr;
    }
}

void _sounds::playShootSound()
{
    ISound* newSound = soundEngine->play2D(shootSoundSource, false, false, true);
    if (newSound)
    {
        newSound->setVolume(0.01f);
        activeSounds.push_back(newSound);
    }
    cleanupSounds();
}

// Remove finished sounds from the active sounds list
void _sounds::cleanupSounds()
{
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
            [](ISound* sound)
            {
                if (!sound || sound->isFinished())
                {
                    if (sound) sound->drop();
                    return true;
                }
                return false;
            }),
        activeSounds.end());
}
