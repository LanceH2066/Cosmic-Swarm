#ifndef _TIMER_H
#define _TIMER_H

#include "_common.h"

class _timer
{
    public:
        _timer();
        virtual ~_timer();

        float getTicks();  // Returns elapsed time in milliseconds
        void update(float deltaTime);  // Accumulate deltaTime (in seconds)
        void pause();
        void unPause();
        void stop();
        void reset();

    private:
        float elapsedTime;  // Accumulated time in seconds
        bool isPaused;      // Track pause state
        float pausedTime;   // Time accumulated before pausing
};

#endif // _TIMER_H
