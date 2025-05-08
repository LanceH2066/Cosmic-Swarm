#ifndef _PARTICLE_SYSTEM_H
#define _PARTICLE_SYSTEM_H

#include <_particle.h>
#include <_textureLoader.h>

class _particleSystem
{
public:
    _particleSystem();
    ~_particleSystem();

    void init(char* textureFile);
    void spawnExplosion(vec3 position, int particleCount, float particleSpeed);
    void update(float deltaTime);
    void draw();

    bool isActive() const { return !particles.empty(); }
    std::vector<Particle> particles;
private:
    _textureLoader* particleTexture;
};

#endif // _PARTICLE_SYSTEM_H
