#include "_timer.h"

_timer::_timer()
{
    elapsedTime = 0.0f;
    pausedTime = 0.0f;
    isPaused = false;
}

_timer::~_timer()
{
}

float _timer::getTicks()
{
    if (isPaused)
    {
        return pausedTime * 1000.0f;  // Return paused time in milliseconds
    }
    return elapsedTime * 1000.0f;  // Convert seconds to milliseconds
}

void _timer::update(float deltaTime)
{
    if (!isPaused)
    {
        elapsedTime += deltaTime;  // Accumulate time in seconds
    }
}

void _timer::pause()
{
    if (!isPaused)
    {
        pausedTime = elapsedTime;
        isPaused = true;
    }
}

void _timer::unPause()
{
    if (isPaused)
    {
        elapsedTime = pausedTime;
        isPaused = false;
    }
}

void _timer::stop()
{
    elapsedTime = 0.0f;
    pausedTime = 0.0f;
    isPaused = false;
}

void _timer::reset()
{
    elapsedTime = 0.0f;
    pausedTime = 0.0f;
    if (isPaused)
    {
        isPaused = false;  // Resetting unpauses the timer
    }
}
