#include <_enemyDrops.h>

_enemyDrops::_enemyDrops()
{
    isActive = false;
    position = {0.0f, 0.0f, 0.0f};
    scale = {1.0f, 1.0f, 1.0f};
    dropTextureLoader = nullptr; // Initialize shared_ptr to nullptr
    xMin = 0.0f;
    xMax = 1.0f;
    yMin = 0.0f;
    yMax = 1.0f;
}

_enemyDrops::~_enemyDrops()
{

}

void _enemyDrops::initDrop(dropType t)
{
    type = t;
    isActive = true;
    timeAlive = 0.0f;
}

void _enemyDrops::drawDrop()
{
    if (!isActive || !dropTextureLoader || dropTextureLoader->tex == 0) return;

    dropTextureLoader->textureBinder(); // Use textureBinder method
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glScalef(scale.x, scale.y, scale.z);
    glBegin(GL_QUADS);
        glTexCoord2f(xMin, yMax); glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(xMax, yMax); glVertex3f( 1.0f, -1.0f, 0.0f);
        glTexCoord2f(xMax, yMin); glVertex3f( 1.0f,  1.0f, 0.0f);
        glTexCoord2f(xMin, yMin); glVertex3f(-1.0f,  1.0f, 0.0f);
    glEnd();
    glPopMatrix();
}

void _enemyDrops::placeDrop(vec3 pos)
{
    position = pos;
    isActive = true;
    timeAlive = 0.0f;
}

void _enemyDrops::update(float deltaTime, vec3 playerPos, float& xpPickupRange, bool& magnetActive, float& magnetTimer, float& playerCurrentHp, float playerMaxHp, bool& playerHasShield, float& shieldTimer)
{
    if (!isActive) return;

    timeAlive += deltaTime;

    // Check pickup
    vec3 diff = {
        playerPos.x - position.x,
        playerPos.y - position.y,
        0.0f
    };

    float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
    float pickupRange = 1.0f;

    if (dist <= pickupRange)
    {
        isActive = false;

        switch (type)
        {
            case MAGNET:
                magnetActive = true;
                xpPickupRange = 1000.0f;
                magnetTimer = std::max(magnetTimer, 2.5f); // Extend or set duration to 2.5 seconds
                break;

            case HEALTH:
                // Heal the player by 20% of maxHp, clamped to maxHp
                playerCurrentHp += playerMaxHp * 0.2f;
                if (playerCurrentHp > playerMaxHp)
                    playerCurrentHp = playerMaxHp;
                break;

            case SHIELD:
                playerHasShield = true;
                shieldTimer = std::max(shieldTimer, 5.0f); // 5-second shield
                break;
        }
    }

    if (timeAlive >= maxLifetime)
    {
        isActive = false;
    }
}

vec3 _enemyDrops::getCollisionBoxMin() const
{
    return {
        position.x - collisionBoxSize.x / 2.0f,
        position.y - collisionBoxSize.y / 2.0f,
        position.z - collisionBoxSize.z / 2.0f
    };
}

vec3 _enemyDrops::getCollisionBoxMax() const
{
    return {
        position.x + collisionBoxSize.x / 2.0f,
        position.y + collisionBoxSize.y / 2.0f,
        position.z + collisionBoxSize.z / 2.0f
    };
}
