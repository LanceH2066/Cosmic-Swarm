#include "_parallax.h"
_parallax::_parallax()
{
    xMax = 1.0;
    xMin = 0.0;
    yMax = 1.0;
    yMin = 0.0;
    speed = 0.005;
    upDown = leftRight = true;
}

_parallax::~_parallax()
{

}

void _parallax::drawBackground(float width, float height, vec3 playerPos)
{
    background->textureBinder();

    float tileSize = 25.0f; // Size of a single tile

    // Calculate the number of tiles needed to cover the screen
    int numTilesX = ceil(width / tileSize) + 2;  // Extra tiles for smooth transitions
    int numTilesY = ceil(height / tileSize) + 2;

    // Find the top-left tile's world position
    float startX = floor(playerPos.x / tileSize) * tileSize;
    float startY = floor(playerPos.y / tileSize) * tileSize;

    glBegin(GL_QUADS);
    for (int i = -numTilesX / 2; i < numTilesX / 2; i++)
    {
        for (int j = -numTilesY / 2; j < numTilesY / 2; j++)
        {
            float tileX = startX + i * tileSize;
            float tileY = startY + j * tileSize;

            glTexCoord2f(0.0f, 0.0f); glVertex3f(tileX, tileY, -1);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(tileX + tileSize, tileY, -1);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(tileX + tileSize, tileY + tileSize, -1);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(tileX, tileY + tileSize, -1);
        }
    }
    glEnd();
}

void _parallax::initParallax(char* fileName, float speed, bool upDown, bool leftRight)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    this->speed = speed;
    this->upDown = upDown;
    this->leftRight = leftRight;
    background->loadTexture(fileName);
}

void _parallax::scroll(string direction)
{

        if(tmr->getTicks()>3)
        {
            if(direction=="up" && upDown==true)
            {
                yMin -= speed;
                yMax -= speed;
            }
            if(direction=="down" && upDown==true)
            {
                yMin += speed;
                yMax += speed;
            }
            if(direction=="left" && leftRight==true)
            {
                xMin -= speed;
                xMax -= speed;
            }
            if(direction=="right" && leftRight==true)
            {
                xMin += speed;
                xMax += speed;
            }
        }
        tmr->reset();

}

void _parallax::autoScroll()
{

        if(tmr->getTicks()>3)
        {
            if(upDown==true)
            {
                yMin += speed;
                yMax += speed;
            }
            if(leftRight==true)
            {
                xMin -= speed;
                xMax -= speed;
            }
        }
        tmr->reset();

}
