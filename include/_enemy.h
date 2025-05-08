#ifndef _ENEMY_H
#define _ENEMY_H
#include <_common.h>
#include <_timer.h>
#include<_textureManager.h>
#include <_particleSystem.h>
#include <_xporb.h>
#include <_sounds.h>

class _enemyDrops;

class _enemy
{
    public:
        _enemy();
        virtual ~_enemy();

        _timer* enemyTimer;
        shared_ptr<_textureLoader> enemyTextureLoader; // Change to shared_ptr
        _particleSystem* explosionEffect;

        vec3 position;
        vec3 rotation;
        vec2 scale;

        vec3 playerPosition;

        float xMax, xMin, yMax, yMin;
        float speed;
        int actionTrigger;
        bool isAlive;

        void initEnemy(EnemyType type, float hp, vec3 hitboxSize, float mSpeed);
        void drawEnemy(GLuint, float);
        void placeEnemy(vec3);
        void enemyActions(float deltaTime);
        void takeDamage(float damage, vector<_xpOrb>& xpOrbs, std::shared_ptr<_textureLoader> xpOrbTexture, vector<_enemyDrops>& enemyDrops, std::shared_ptr<_textureLoader> enemyDropsMagnetTexture, std::shared_ptr<_textureLoader> enemyDropsHealthTexture,_sounds *sounds);

        float maxHp, currentHp;
        float stoppingDistance = 0.35f;
        vec3 collisionBoxSize;
        vec3 getCollisionBoxMin() const { return {position.x - collisionBoxSize.x, position.y - collisionBoxSize.y, position.z - collisionBoxSize.z}; }
        vec3 getCollisionBoxMax() const { return {position.x + collisionBoxSize.x, position.y + collisionBoxSize.y, position.z + collisionBoxSize.z}; }

        vector<vec3> getRotatedCorners() const;

        bool startFlash;
        float flashDuration = 0.2f;
        float flashTimer = 0.0f;
        bool hasExploded;
        bool isBoss;
        AABB getAABB() const;

    protected:
    private:
};

#endif // _ENEMY_H
