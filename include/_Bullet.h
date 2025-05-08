#ifndef _BULLET_H
#define _BULLET_H

#include<_common.h>
#include<_timer.h>
#include<_enemy.h>
#include<_weapon.h>

class _Bullet
{
    public:
    _Bullet();
    virtual ~_Bullet();
    shared_ptr<_textureLoader> textureLoader;
    _particleSystem* explosionEffect;
    float xMin, xMax, yMin, yMax;
    int t = 0;
    enum {IDLE, SHOOT, HIT};
    bool hasExploded;
    Weapon weapon;
    float laserLength; // Length of the beam for lasers
    int actionTrigger;
    bool isAlive = false;
    vec3 initialPosition, position, direction, rotation, scale;
    vec3 collisionBoxSize; // Dynamic size set in init
    vec3 baseCollisionBoxSize = {0.05f, 0.25f, 1.0f}; // Base size for non-laser bullets
    void init(vec3 spawnPos, vec3 playerRotation, vec3 targetPos, shared_ptr<_textureLoader> tex, Weapon wep);
    void reset(vec3);
    void update(float deltaTime, vector<_enemy>& enemies); // Add enemies for AoE processing
    void actions();
    void drawBullet(float deltaTime);
    vec3 getCollisionBoxMin() const;
    vec3 getCollisionBoxMax() const;
    void explode(vector<_enemy>& enemies, vector<_xpOrb>& xpOrbs, std::shared_ptr<_textureLoader> xpOrbTexture, vector<_enemyDrops>& enemyDrops, std::shared_ptr<_textureLoader> enemyDropsMagnetTexture, std::shared_ptr<_textureLoader> enemyDropsHealthTexture,_sounds *sounds);
    static const float laserHitboxLengths[5];
    vector<vec3> getRotatedCorners() const;
    vector<int> hitEnemies;
    AABB getAABB() const;  // Moved from protected to public
    float explosionTimer = 0.0f;
float maxExplosionDuration = 3.0f; // Duration to keep explosion effect (adjust as needed)
    protected:

    private:
};

#endif // _BULLET_H
