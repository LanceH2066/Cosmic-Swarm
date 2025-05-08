#include "_enemy.h"
#include<_enemyDrops.h>

_enemy::_enemy()
{
    position.x = 0.0; position.y = 0.0; position.z = -2.0;
    scale.x = 1.0; scale.y = 1.0;
    rotation.x = 0.0; rotation.y = 0.0; rotation.z = 0.0;
    speed = 2.0f;
    isAlive = false;
    startFlash = false;
    xMin = 0;
    xMax = 1.0;
    yMax = 1.0;
    yMin = 0;
    currentHp = maxHp = 20.0f;
    explosionEffect = new _particleSystem();
    hasExploded = false;
    playerPosition = {0.0f, 0.0f, 0.0f}; // Initialize
    enemyTextureLoader = nullptr; // Initialize shared_ptr to nullptr
}

_enemy::~_enemy()
{

}

void _enemy::initEnemy(EnemyType type, float hp, vec3 hitboxSize, float mSpeed) {
    if (type == SWARMBOT) enemyTextureLoader = _textureManager::getInstance().getTexture("swarmbot");
    else if (type == BUGSHIP) enemyTextureLoader = _textureManager::getInstance().getTexture("bugShip");
    else if (type == SMALLBUG) enemyTextureLoader = _textureManager::getInstance().getTexture("smallbug");
    else if (type == BOSSSHIP) enemyTextureLoader = _textureManager::getInstance().getTexture("bossShip");

    if (!enemyTextureLoader) std::cerr << "Enemy texture not found!" << std::endl;
    explosionEffect->init("images/particle.png");
    maxHp = currentHp = hp;
    collisionBoxSize = hitboxSize;
    speed = mSpeed;
}

void _enemy::drawEnemy(GLuint tex, float deltaTime) {
    if (isAlive) {
        glPushMatrix();
            glColor3f(1.0, 1.0, 1.0);
            if (enemyTextureLoader) {
                enemyTextureLoader->textureBinder();
            }
            glTranslatef(position.x, position.y, position.z);
            glRotatef(rotation.x, 1, 0, 0);
            glRotatef(rotation.y, 0, 1, 0);
            glRotatef(rotation.z, 0, 0, 1);
            glScalef(scale.x, scale.y, 1.0);

            glBegin(GL_QUADS);
                glTexCoord2f(xMin, yMin); glVertex3f(1.0, 1.0, 0);
                glTexCoord2f(xMax, yMin); glVertex3f(-1.0, 1.0, 0);
                glTexCoord2f(xMax, yMax); glVertex3f(-1.0, -1.0, 0);
                glTexCoord2f(xMin, yMax); glVertex3f(1.0, -1.0, 0);
            glEnd();
        glPopMatrix();
    }
    if (explosionEffect->isActive()) {
        explosionEffect->update(deltaTime);
        explosionEffect->draw();
    }
}

void _enemy::placeEnemy(vec3 pos)
{
    position.x = pos.x;
    position.y = pos.y;
    position.z = pos.z;
    hasExploded = false;
}

void _enemy::takeDamage(float damage, vector<_xpOrb>& xpOrbs, std::shared_ptr<_textureLoader> xpOrbTexture, vector<_enemyDrops>& enemyDrops, std::shared_ptr<_textureLoader> enemyDropsMagnetTexture, std::shared_ptr<_textureLoader> enemyDropsHealthTexture,_sounds *sounds)
{
    if (sounds) sounds->playEnemyDamage();

    currentHp -= damage;
    if (currentHp <= 0 && isAlive)
    {
        isAlive = false;
        if (!hasExploded)
        {
            explosionEffect->spawnExplosion(position, 15, 0);
            hasExploded = true;
            if (sounds) sounds->playEnemyDeath();
        }

        _xpOrb orb;
        orb.xpTextureLoader = xpOrbTexture;
        orb.placeOrb(position); // Use enemy's position
        orb.initOrb();
        xpOrbs.push_back(orb);

        // Dynamic drop rate based on gameTime
        const int m_start = 10;        // 1/50 = 2% chance at start
        const int m_end = 300;         // 1/200 = 0.5% chance at end
        const float time_to_reach_end = 300.0f;  // 10 minutes

        int m = m_start + static_cast<int>((gameTime / time_to_reach_end) * (m_end - m_start));
        m = std::min(m, m_end);  // Cap at m_end

        if (rand() % m == 0)
        {

            _enemyDrops drop;
            int dropRand = rand() % 3;
            _enemyDrops::dropType type;
            if (dropRand == 0) type = _enemyDrops::MAGNET;
            else if (dropRand == 1) type = _enemyDrops::HEALTH;
            else type = _enemyDrops::SHIELD;

            drop.initDrop(type);
            if (type == _enemyDrops::MAGNET)
            drop.dropTextureLoader = enemyDropsMagnetTexture;
            else if (type == _enemyDrops::HEALTH)
            drop.dropTextureLoader = enemyDropsHealthTexture;
            else
            drop.dropTextureLoader = _textureManager::getInstance().getTexture("shield");

            drop.placeDrop(position);
            enemyDrops.push_back(drop);


        }
    }
    startFlash = true;
    flashTimer = 0.0f;  // Reset flash timer

}

void _enemy::enemyActions(float deltaTime)
{
    // Handle flash effect
    if (startFlash)
    {
        flashTimer += deltaTime;
        if (flashTimer >= flashDuration)
        {
            startFlash = false;  // Revert to default sprite
            flashTimer = 0.0f;  // Reset timer
        }
    }

    if(!startFlash) // default sprite
    {
        xMin = 0;
        xMax = 0.5f;
    }
    else    // white sprite for flash
    {
        xMin = 0.5f;
        xMax = 1.0f;
    }

    // Compute direction vector from enemy to player
    float deltaX = playerPosition.x - position.x;
    float deltaY = playerPosition.y - position.y;

    // Compute the distance to the player
    float magnitude = sqrt(deltaX * deltaX + deltaY * deltaY);

    // Normalize direction vector
    if (magnitude > 0.0f)
    {
        deltaX /= magnitude;
        deltaY /= magnitude;
    }

    // Move enemy towards the player only if outside the stopping distance
    if (magnitude > stoppingDistance)
    {
        position.x += deltaX * speed * deltaTime;
        position.y += deltaY * speed * deltaTime;
    }
}

vector<vec3> _enemy::getRotatedCorners() const
{
    vector<vec3> corners(4);
    vec3 min = getCollisionBoxMin();
    vec3 max = getCollisionBoxMax();

    corners[0] = {min.x - position.x, min.y - position.y, 0};  // Bottom-left
    corners[1] = {max.x - position.x, min.y - position.y, 0};  // Bottom-right
    corners[2] = {max.x - position.x, max.y - position.y, 0};  // Top-right
    corners[3] = {min.x - position.x, max.y - position.y, 0};  // Top-left

    float angleRad = rotation.z * (M_PI / 180.0);
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);

    for (auto& corner : corners)
    {
        float x = corner.x * cosA - corner.y * sinA;
        float y = corner.x * sinA + corner.y * cosA;
        corner.x = x + position.x;
        corner.y = y + position.y;
    }

    return corners;
}
AABB _enemy::getAABB() const {
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
