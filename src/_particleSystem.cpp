#include "_particleSystem.h"

_particleSystem::_particleSystem() {
    particleTexture = new _textureLoader();
}

_particleSystem::~_particleSystem() {
    delete particleTexture;
}

void _particleSystem::init(char* textureFile) {
    particleTexture->loadTexture(textureFile); // Load particle texture (e.g., a spark or glow)
    if (particleTexture->tex == 0) {
        std::cerr << "[ERROR] Failed to load particle texture: " << textureFile << std::endl;
    }
}

void _particleSystem::spawnExplosion(vec3 position, int particleCount, float particleSpeed)
{
    particles.clear();
    for (int i = 0; i < particleCount; ++i)
    {
        Particle p;
        p.position = position;
        float angle = (rand() % 360) * (M_PI / 180.0f);
        float speed = particleSpeed;
        p.velocity.x = cos(angle) * speed;
        p.velocity.y = sin(angle) * speed;
        p.velocity.z = 0;

        p.lifetime = 1.0f + (rand() % 100) / 100.0f;
        p.maxLifetime = p.lifetime;

        p.color = vec4(0.8, 0.8, 0.8, 0.5f);
        p.size = 0.80f + (rand() % 50) / 100.0f;

        particles.push_back(p);
    }
}

void _particleSystem::update(float deltaTime)
{
    for (auto it = particles.begin(); it != particles.end();)
    {
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0)
        {
            it = particles.erase(it);
        }
        else
        {
            // Update position
            it->position.x += it->velocity.x * deltaTime;
            it->position.y += it->velocity.y * deltaTime;

            // Fade out
            float alpha = it->lifetime / it->maxLifetime;
            it->color.w = alpha;

            // Slow down over time
            float decay = pow(0.99f, deltaTime);
            it->velocity.x *= decay;
            it->velocity.y *= decay;

            ++it;
        }
    }
}

void _particleSystem::draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for bright explosion
    glDepthMask(GL_FALSE); // Disable depth writing to prevent z-fighting
    particleTexture->textureBinder();

    for (const auto& p : particles)
    {
        glPushMatrix();
        glTranslatef(p.position.x, p.position.y, p.position.z);
        glScalef(p.size, p.size, 1.0f);
        glColor4f(p.color.x, p.color.y, p.color.z, p.color.w);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
        glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
        glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
        glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
        glEnd();

        glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Restore scene�s blending

    glColor4f(1.0,1.0,1.0,1.0);
}
