#ifndef _ENEMYDROPS_H
#define _ENEMYDROPS_H
#include <_common.h>
#include <_textureLoader.h>

class _enemyDrops
{
    public:
        _enemyDrops();
        virtual ~_enemyDrops();

        enum dropType {MAGNET, HEALTH, SHIELD};

        // Properties
        vec3 position;
        vec3 scale;
        bool isActive;
        dropType type;

        float timeAlive = 0.0f;
        float maxLifetime = 60.0f; // Time before disappearing

        // Animation/texture
        float xMin, xMax, yMin, yMax;
        std::shared_ptr<_textureLoader> dropTextureLoader; // Use shared_ptr

        vec3 collisionBoxSize = {0.4f, 0.4f, 1.0f};

        // Methods
        void initDrop(dropType t);
        void drawDrop();
        void placeDrop(vec3 pos);
        void update(float deltaTime, vec3 playerPos, float& xpPickupRange, bool& magnetActive, float& magnetTimer, float& playerCurrentHp, float playerMaxHp, bool& playerHasShield, float& shieldTimer);
        vec3 getCollisionBoxMin() const;
        vec3 getCollisionBoxMax() const;

    protected:
    private:
};
#endif // _ENEMYDROPS_H
