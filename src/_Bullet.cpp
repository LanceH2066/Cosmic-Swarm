#include "_Bullet.h"
const float _Bullet::laserHitboxLengths[5] = {2.3f, 2.9f, 3.5f, 4.3f, 5.1f};

_Bullet::_Bullet()
{
    hasExploded = false;
    explosionEffect = new _particleSystem();
    hitEnemies.clear();
}

_Bullet::~_Bullet()
{

}

void _Bullet::init(vec3 spawnPos, vec3 playerRotation, vec3 targetPos, shared_ptr<_textureLoader> tex, Weapon wep) {
    if (!tex) std::cout << "Trying to init bullet with null texture!" << std::endl;

    weapon = wep;
    initialPosition = position = spawnPos;
    position.z = spawnPos.z + 1;
    explosionEffect->init("images/rocketParticle.png");

    if (weapon.type == LASER)
    {
        // Lasers are beams attached to the player
        laserLength = round(10.0*weapon.level);
        direction = {0, 0, 0}; // Direction handled by player rotation
        collisionBoxSize = {0.1f, laserHitboxLengths[weapon.level-1], 1.0f}; // Thin, long hitbox
        isAlive = true;
    }
    else if (weapon.type == ENERGY_FIELD)
    {
        // Energy field follows player, no direction
        direction = {0, 0, 0};
        collisionBoxSize = {weapon.aoeSize, weapon.aoeSize, 1.0f};
        isAlive = true;
    }
    else if(weapon.type == DEFAULT)
    {
        float angleRad = (playerRotation.z + 90) * (M_PI / 180.0);
        direction.x = cos(angleRad);
        direction.y = sin(angleRad);
        collisionBoxSize = {baseCollisionBoxSize.x * weapon.aoeSize, baseCollisionBoxSize.y * weapon.aoeSize, 1.0f};
    }
    else if(weapon.type == ROCKET)
    {
        float angleRad = (playerRotation.z + 90) * (M_PI / 180.0);
        direction.x = cos(angleRad);
        direction.y = sin(angleRad);
        collisionBoxSize = {weapon.aoeSize*0.25, 0.25*weapon.aoeSize, 1.0f};
    }
    else
    {
        float angleRad = (playerRotation.z + 90) * (M_PI / 180.0);
        direction.x = cos(angleRad);
        direction.y = sin(angleRad);
        collisionBoxSize = {weapon.aoeSize*0.6, 0.6*weapon.aoeSize, 1.0f};
    }
    scale = {1, 1, 1};
    rotation = playerRotation;
    xMin = yMin = 0;
    xMax = yMax = 1.0;
    textureLoader = tex;
    hitEnemies.clear();
}

void _Bullet::reset(vec3 playerPos)
{
    position = playerPos;
    isAlive = false;
    hitEnemies.clear();
}

void _Bullet::update(float deltaTime, vector<_enemy>& enemies)
{
    if (!isAlive || hasExploded) {
        if (hasExploded) {
            explosionTimer += deltaTime;
            if (explosionTimer >= maxExplosionDuration) {
                isAlive = false; // Mark bullet as dead after explosion duration
            }
        }
        return;
    }

    if (weapon.type == DEFAULT || weapon.type == ROCKET || weapon.type == FLAK)
    {
        position.x += direction.x * weapon.projSpeed * deltaTime;
        position.y += direction.y * weapon.projSpeed * deltaTime;

        float maxTravelDistance = 100.0f;
        float distanceSq = (position.x - initialPosition.x) * (position.x - initialPosition.x) +
                           (position.y - initialPosition.y) * (position.y - initialPosition.y);
        if (distanceSq > maxTravelDistance * maxTravelDistance)
        {
            isAlive = false;
        }
    }
    /*
    else if (weapon.type == ENERGY_FIELD)
    {
        // Energy field weapon.damages enemies in radius
        for (auto& enemy : enemies)
        {
            float dx = enemy.position.x - position.x;
            float dy = enemy.position.y - position.y;
            if (sqrt(dx * dx + dy * dy) <= weapon.aoeSize)
            {
                enemy.takeDamage(weapon.damage * deltaTime); // weapon.damage over time
            }
        }
    }
    */
}

void _Bullet::explode(vector<_enemy>& enemies, vector<_xpOrb>& xpOrbs, std::shared_ptr<_textureLoader> xpOrbTexture, vector<_enemyDrops>& enemyDrops, std::shared_ptr<_textureLoader> enemyDropsMagnetTexture, std::shared_ptr<_textureLoader> enemyDropsHealthTexture,_sounds *sounds)
{
    hasExploded = true;
    explosionTimer = 0.0f;

    // Use aoeSize to drive visual effect and gameplay
    int particleCount = static_cast<int>(weapon.aoeSize * 100); // adjust if too many/few
    float particleSpeed = weapon.aoeSize*1.25;
    explosionEffect->spawnExplosion(position, particleCount, particleSpeed);
    sounds->play(sounds->rocketExplosionSource);

    // Damage enemies within radius defined by aoeSize
    for (auto& enemy : enemies) {
        float dx = enemy.position.x - position.x;
        float dy = enemy.position.y - position.y;
        if (sqrt(dx * dx + dy * dy) <= weapon.aoeSize*3.0f) {
            enemy.takeDamage(weapon.damage, xpOrbs, xpOrbTexture, enemyDrops, enemyDropsMagnetTexture, enemyDropsHealthTexture, sounds);
        }
    }
}

void _Bullet::actions()
{
    switch(actionTrigger)
    {
        case IDLE:
        {
            isAlive = false;
            break;
        }
        case SHOOT:
        {
            isAlive = true;
            break;
        }
        case HIT:
        {
            isAlive = false;
            break;
        }
    }
}

void _Bullet::drawBullet(float deltaTime)
{
    glPushMatrix();
    if (isAlive && !hasExploded)
    {
        if (weapon.type == LASER)
        {
            collisionBoxSize = {0.1f, laserHitboxLengths[weapon.level-1], 1.0f};

            glTranslatef(position.x, position.y, position.z);
            glRotatef(rotation.z, 0.0f, 0.0f, 1.0f); // Counter-rotate by 90 degrees

            laserLength = round(5.0*weapon.level+weapon.aoeSize);
            int numTiles = laserLength;
            float tileHeight = 0.4f;
            for (int i = 0; i < numTiles; ++i)
            {
                glPushMatrix();
                glTranslatef(0.0, i * tileHeight, 0.0); // Offset each tile upward
                glScalef(1.0f, 1.0f, 1.0f); // Consistent scale per tile
                glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // Counter-rotate by 90 degrees

                if (textureLoader && isAlive)
                {
                    textureLoader->textureBinder();
                }

                glBegin(GL_QUADS);
                    glTexCoord2f(xMin, yMax); glVertex3f(-1, -1, 0);
                    glTexCoord2f(xMax, yMax); glVertex3f( 1, -1, 0);
                    glTexCoord2f(xMax, yMin); glVertex3f( 1,  1, 0);
                    glTexCoord2f(xMin, yMin); glVertex3f(-1,  1, 0);
                glEnd();
                glPopMatrix();
            }
        }
        else
        {
            if(weapon.type== ROCKET) scale.x = scale.y = 0.5f*weapon.aoeSize;
            else scale.x = scale.y = weapon.aoeSize;

            glTranslatef(position.x, position.y, position.z);
            glRotatef(rotation.z - 90.0f, 0.0f, 0.0f, 1.0f); // Counter-rotate by 90 degrees
            glScalef(scale.x, scale.y, scale.z);

            if (textureLoader && isAlive)
            {
                textureLoader->textureBinder();
            }


            glBegin(GL_QUADS);
                glTexCoord2f(xMin, yMax);
                glVertex3f(-1, -1, 0);
                glTexCoord2f(xMax, yMax);
                glVertex3f(1, -1, 0);
                glTexCoord2f(xMax, yMin);
                glVertex3f(1, 1, 0);
                glTexCoord2f(xMin, yMin);
                glVertex3f(-1, 1, 0);
            glEnd();
        }

    }
    glPopMatrix();
    if (explosionEffect->isActive())
    {
        explosionEffect->update(deltaTime);
        explosionEffect->draw();
    }
}

vec3 _Bullet::getCollisionBoxMin() const
{
    if (weapon.type == LASER)
    {
        // Hitbox starts at position (ship's front)
        return {position.x - collisionBoxSize.x,
                position.y,
                position.z - collisionBoxSize.z};
    }
    return {position.x - collisionBoxSize.x,
            position.y - collisionBoxSize.y,
            position.z - collisionBoxSize.z};
}

vec3 _Bullet::getCollisionBoxMax() const
{
    if (weapon.type == LASER)
    {
        // Extend forward by full laserLength
        return {position.x + collisionBoxSize.x,
                position.y + collisionBoxSize.y * 2.0f,
                position.z + collisionBoxSize.z};
    }
    return {position.x + collisionBoxSize.x,
            position.y + collisionBoxSize.y,
            position.z + collisionBoxSize.z};
}

vector<vec3> _Bullet::getRotatedCorners() const
{
    vector<vec3> corners(4);
    float angleRad = rotation.z * (M_PI / 180.0);
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);

    if (weapon.type == LASER)
    {
        float halfWidth = collisionBoxSize.x;
        float halfHeight = collisionBoxSize.y;

        // Define corners: laser extends forward from position
        corners[0] = {-halfWidth, 0, 0};           // Bottom-left (at position)
        corners[1] = {halfWidth, 0, 0};           // Bottom-right
        corners[2] = {halfWidth, halfHeight * 2.0f, 0}; // Top-right (full length)
        corners[3] = {-halfWidth, halfHeight * 2.0f, 0}; // Top-left

        for (auto& corner : corners)
        {
            float x = corner.x * cosA - corner.y * sinA;
            float y = corner.x * sinA + corner.y * cosA;
            corner.x = x + position.x;
            corner.y = y + position.y;
        }
    }
    else
    {
        vec3 min = getCollisionBoxMin();
        vec3 max = getCollisionBoxMax();

        corners[0] = {min.x - position.x, min.y - position.y, 0}; // Bottom-left
        corners[1] = {max.x - position.x, min.y - position.y, 0}; // Bottom-right
        corners[2] = {max.x - position.x, max.y - position.y, 0}; // Top-right
        corners[3] = {min.x - position.x, max.y - position.y, 0}; // Top-left

        for (auto& corner : corners)
        {
            float x = corner.x * cosA - corner.y * sinA;
            float y = corner.x * sinA + corner.y * cosA;
            corner.x = x + position.x;
            corner.y = y + position.y;
        }
    }

    return corners;
}
AABB _Bullet::getAABB() const {
    std::vector<vec3> corners = getRotatedCorners();
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    for (const auto& corner : corners) {
        minX = std::min(minX, corner.x);
        minY = std::min(minY, corner.y);
        maxX = std::max(maxX, corner.x);
        maxY = std::max(maxY, corner.y);
    }
    return {minX, minY, maxX, maxY};
}
