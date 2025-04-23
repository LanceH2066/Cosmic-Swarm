#include "_inputs.h"

_inputs::_inputs()
{

}

_inputs::~_inputs()
{

}

void _inputs::keyPressed(_player* player, _sounds* sounds,float deltaTime)
{
    player->actionTrigger = player->FLYING;

    bool moveLeft = GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000;
    bool moveRight = GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000;
    bool moveUp = GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000;
    bool moveDown = GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000;

    float moveX = 0.0f, moveY = 0.0f;

    if (moveLeft) moveX = -1.0f;
    if (moveRight) moveX = 1.0f;
    if (moveUp) moveY = 1.0f;
    if (moveDown) moveY = -1.0f;

    // Normalize diagonal movement
    if (moveX != 0.0f && moveY != 0.0f)
    {
        float magnitude = sqrt(moveX * moveX + moveY * moveY);
        moveX /= magnitude;
        moveY /= magnitude;
    }

    player->playerPosition.x += moveX * player->speed * deltaTime;
    player->playerPosition.y += moveY * player->speed * deltaTime;

    if (moveLeft || moveRight || moveUp || moveDown)
    {
        //sounds->playThrusterSound();
    }
}

void _inputs::keyUp(_player* player, _sounds* sounds)
{
    bool moveLeft = GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000;
    bool moveRight = GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000;
    bool moveUp = GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000;
    bool moveDown = GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000;

    if (!moveLeft && !moveRight && !moveUp && !moveDown)
    {
        player->actionTrigger = player->IDLE;
        //sounds->stopThrusterSound();
    }
}
void _inputs::updateMouseRotation(_player* player, int mouseX, int mouseY, int screenWidth, int screenHeight)
{
    // Compute direction vector from player to mouse using SCREEN coordinates
    float deltaX = mouseX - (screenWidth / 2);
    float deltaY = (screenHeight / 2) - mouseY; // Invert Y-axis

    // Compute the angle using atan2()
    float angle = atan2(deltaY, deltaX) * (180.0f / M_PI);
    player->playerRotation.z = angle - 90; // Adjust for sprite alignment
}


