#ifndef _PARTICLE_H
#define _PARTICLE_H

#include <_common.h>

struct Particle
{
    vec3 position;    // Current position
    vec3 velocity;    // Movement direction and speed
    float lifetime;   // Time left before particle disappears
    float maxLifetime; // Total lifetime for fading
    vec4 color;       // RGBA color for tinting/fading
    float size;       // Particle size (for scaling quad)

    Particle() : position(0, 0, 0), velocity(0, 0, 0), lifetime(0), maxLifetime(1), color(1, 1, 1, 1), size(0.1f) {}
};

#endif // _PARTICLE_H
