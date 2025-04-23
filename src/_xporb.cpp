#include "_xpOrb.h"

_xpOrb::_xpOrb()
{
    //ctor
    position = vec3(0.0f, 0.0f, 0.0f);
    scale = vec3(0.25f, 0.25f, 1.0f);
    isActive = true;

    xMin = 0.0f;
    xMax = 1.0f;
    yMin = 0.0f;
    yMax = 1.0f;
    xpTextureLoader = nullptr; // Initialize shared_ptr to nullptr
}

_xpOrb::~_xpOrb()
{
    //dtor
}

void _xpOrb::initOrb()
{
    // Texture is assigned externally (e.g., by _scene via _textureManager)
    if (!xpTextureLoader) {
        std::cerr << "Error: xpTextureLoader is null in initOrb!" << std::endl;
    }
    isActive = true;
}

void _xpOrb::drawOrb()
{
    if (!isActive) return;

    glPushMatrix();

        glTranslatef(position.x, position.y, position.z);
        glScalef(scale.x, scale.y, 1.0f);
        glColor3f(1.0f, 1.0f, 1.0f);

        xpTextureLoader->textureBinder();

        glBegin(GL_QUADS);
            glTexCoord2f(xMin, yMin); glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(xMax, yMin); glVertex3f(1.0f, -1.0f, 0.0f);
            glTexCoord2f(xMax, yMax); glVertex3f(1.0f, 1.0f, 0.0f);
            glTexCoord2f(xMin, yMax); glVertex3f(-1.0f, 1.0f, 0.0f);
        glEnd();

    glPopMatrix();
}

void _xpOrb::update(float deltaTime, vec3 playerPos, float pickupRange)
{
    if (!isActive) return;

    if(pickupRange >=1000)
    {
        speed = 15.0f;
    }
    else
    {
        speed = 2.0f;
    }
    // Calculate distance to player
    vec3 diff = {playerPos.x - position.x, playerPos.y - position.y, 0.0f};
    float distance = sqrt(diff.x * diff.x + diff.y * diff.y);

    // Move toward player if within pickup range
    if (distance <= pickupRange && distance > 0.0f)
    {
        // Normalize direction
        diff.x /= distance;
        diff.y /= distance;

        // Update position
        position.x += diff.x * speed * deltaTime;
        position.y += diff.y * speed * deltaTime;
    }
}

void _xpOrb::placeOrb(vec3 pos)
{
     position = pos;
}

vec3 _xpOrb::getCollisionBoxMin() const
{
        return {
        position.x - collisionBoxSize.x,
        position.y - collisionBoxSize.y,
        position.z - collisionBoxSize.z
    };
}

vec3 _xpOrb::getCollisionBoxMax() const
{
        return {
        position.x + collisionBoxSize.x,
        position.y + collisionBoxSize.y,
        position.z + collisionBoxSize.z
    };
}

