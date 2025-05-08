#include "_scene.h"

_scene::_scene(){
    input = new _inputs();
    prlx1 = new _parallax();
    mainmenu = new _parallax();
    player = new _player();
    collision = new _collision();
    gameOverExplosion = new _particleSystem();
    sounds = new _sounds();
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);
    deltaTime = 0.0f;
    enemies.clear();
}

_scene::~_scene(){
    delete input;
    delete prlx1;
    delete player;
    delete collision;
    delete sounds;
    delete gameOverExplosion;
    delete mainmenu;
    input = nullptr;
    prlx1 = nullptr;
    player = nullptr;
    collision = nullptr;
    sounds = nullptr;
    mainmenu = nullptr;
    gameOverExplosion = nullptr;
}

GLint _scene::initGL() {
    srand(static_cast<unsigned>(time(nullptr)));
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClearDepth(1.0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    dim.x = GetSystemMetrics(SM_CXSCREEN);
    dim.y = GetSystemMetrics(SM_CYSCREEN);
    scaleX = dim.x / refWidth;
    scaleY = dim.y / refHeight;
    // Preload all textures
    _textureManager::getInstance().preloadTextures();

    // Initialize objects
    prlx1->initParallax("images/background.png", 0.005, false, false);
    mainmenu->initParallax("images/mainmenu.png", 0.005, false, false);
    player->initPlayer(1, 1, nullptr);
    // Assign shared_ptr directly
    xpOrbTexture = _textureManager::getInstance().getTexture("xpOrb");
    enemyDropsMagnetTexture = _textureManager::getInstance().getTexture("magnet");
    enemyDropsHealthTexture = _textureManager::getInstance().getTexture("healthDrop");
    hudTexture = _textureManager::getInstance().getTexture("hud");
    fontTexture = _textureManager::getInstance().getTexture("font");
    damageIconTexture = _textureManager::getInstance().getTexture("damage");
    fireRateIconTexture = _textureManager::getInstance().getTexture("fireRate");
    aoeSizeIconTexture = _textureManager::getInstance().getTexture("aoeSize");
    speedIconTexture = _textureManager::getInstance().getTexture("speed");
    healthIconTexture = _textureManager::getInstance().getTexture("health");
    enemyDropsShieldTexture = _textureManager::getInstance().getTexture("shield");

    loadFontData("images/font.fnt");
    gameOverExplosion->init("images/particle.png");
    float worldUnitsPerPixel = 10.0f / dim.x;
    float screenWidthUnits = dim.x * worldUnitsPerPixel;
    float minDistance = screenWidthUnits * 1.2f;
    float maxDistance = screenWidthUnits * 1.5f;

    bossEnemy.initEnemy(BOSSSHIP, 50.0f, {2.0f, 2.0f, 1.0f}, 4.5f);
    bossEnemy.isAlive = false;

    initMenus();
    sounds->playMusic();
    startGame();

    return true;
}

void _scene::initMenus() {
    float buttonWidth = 300.0f * scaleX;
    float buttonHeight = 80.0f * scaleY;
    float startY = dim.y * 0.6f;
    float spacing = 120.0f * scaleY;

    mainMenuButtons.clear();
    mainMenuButtons.push_back({(dim.x - buttonWidth) / 2.0f, startY*0.9 - 0*spacing, buttonWidth, buttonHeight, "Play", [this]() { startGame(); currentState = PLAYING; }});
    mainMenuButtons.push_back({(dim.x - buttonWidth) / 2.0f, startY*0.9 - 1*spacing, buttonWidth, buttonHeight, "Help", [this]() { previousState = MAIN_MENU; currentState = HELP_SCREEN; }});
    mainMenuButtons.push_back({(dim.x - buttonWidth) / 2.0f, startY*0.9 - 2*spacing, buttonWidth, buttonHeight, "Credits", [this]() { previousState = MAIN_MENU; currentState = CREDITS_SCREEN; }});
    mainMenuButtons.push_back({(dim.x - buttonWidth) / 2.0f, startY*0.9 - 3*spacing, buttonWidth, buttonHeight, "Quit", []() { PostQuitMessage(0); }});

    pauseMenuButtons.clear();
    pauseMenuButtons.push_back({(dim.x - buttonWidth) / 2.0f, startY - 0*spacing, buttonWidth, buttonHeight, "Resume", [this]() { currentState = PLAYING; }});
    pauseMenuButtons.push_back({(dim.x - buttonWidth) / 2.0f, startY - 1*spacing, buttonWidth, buttonHeight, "Help", [this]() { previousState = PAUSED; currentState = HELP_SCREEN; }});
    pauseMenuButtons.push_back({(dim.x - buttonWidth) / 2.0f, startY - 2*spacing, buttonWidth, buttonHeight, "Main Menu", [this]() { currentState = MAIN_MENU; startGame(); }});

    gameOverButtons.clear();
    gameOverButtons.push_back({(dim.x - buttonWidth) / 2.0f, startY - 0*spacing, buttonWidth, buttonHeight, "Play Again", [this]() { startGame(); currentState = PLAYING; }});
    gameOverButtons.push_back({(dim.x - buttonWidth) / 2.0f, startY - 1*spacing, buttonWidth, buttonHeight, "Main Menu", [this]() { currentState = MAIN_MENU; }});

    helpScreenButtons.clear();
    helpScreenButtons.push_back({(dim.x - buttonWidth) / 2.0f, dim.y * 0.2f, buttonWidth, buttonHeight, "Back", [this]() { currentState = previousState; }});

    creditsScreenButtons.clear();
    creditsScreenButtons.push_back({(dim.x - buttonWidth) / 2.0f, dim.y * 0.2f, buttonWidth, buttonHeight, "Back", [this]() { currentState = previousState; }});
}

void _scene::startGame() {
    gameTime  = 0;
    player->playerPosition = {0.0f, 0.0f, 50.0f};
    player->currentHp = player->maxHp = 100.0f;

    player->playerLevel = 1;
    player->experiencePoints = 0;
    player->xpThresh = player->calculateXPThreshold(player->playerLevel);

    // Reset upgrade multipliers
    player->damageMultiplier = 1.0f;
    player->speedMultiplier = 1.0f;
    player->healthMultiplier = 1.0f;
    player->fireRateMultiplier = 1.0f;
    player->aoeSizeMultiplier = 1.0f;

    // Reset other player states
    player->magnetActive = false;
    player->xpPickupRange = 5.0f;
    player->startFlash = false;
    player->flashTimer = 0.0f;

    player->weapons.clear();
    Weapon defaultWeapon;
    defaultWeapon.init(DEFAULT, player->bulletTextureLoader, 5.0f, 0.2f, 1.0f, 30.0f);
    player->weapons.push_back(defaultWeapon);
    player->bullets.clear();
    clearScreenDone = false;
    enemies.clear();
    xpOrbs.clear();
    enemyDrops.clear();
    elapsedTime = 0.0f;
    lastSpawnTime = 0.0f;
    lastBugSpawnTime = 0.0f;
    damageCooldown = 0.0f;
    isPaused = false;
    gameOver = false;
    upgradeMenuActive = false;
    bossSpawned = false;
    bossDefeated = false;
    hasPlayedEndingExplosion = false;
    activeUpgrades.clear();
}

void _scene::drawButton(const Button& button) {
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.5725f, 0.8784f, 0.9137f, 0.5f); // #92e0e9 with 50% transparency
    glBegin(GL_QUADS);
        glVertex2f(button.x, button.y);
        glVertex2f(button.x + button.width, button.y);
        glVertex2f(button.x + button.width, button.y + button.height);
        glVertex2f(button.x, button.y + button.height);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    float textScale = 1.0f * scaleY;
    float textWidth = calculateTextWidth(button.label, textScale);
    float textX = button.x + (button.width - textWidth) / 2.0f;
    float textY = button.y + (button.height / 2.0f);
    renderText(button.label, textX, textY, textScale, 0.0f);
}

float _scene::calculateTextWidth(const string& text, float scale) {
    float width = 0.0f;
    for (char c : text) {
        int ascii = static_cast<unsigned char>(c);
        if (bitmapFont.find(ascii) != bitmapFont.end()) {
            width += bitmapFont[ascii].xadvance * scale;
        }
    }
    return width;
}

void _scene::drawMainMenu() {
    for (const auto& button : mainMenuButtons) {
        drawButton(button);
    }
}

void _scene::drawPauseMenu() {
    // Switch to projection matrix and set orthographic projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, dim.x, 0, dim.y);  // (0,0) bottom-left, (dim.x, dim.y) top-right

    // Switch to modelview matrix and reset it
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw semi-transparent background
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(dim.x, 0);
        glVertex2f(dim.x, dim.y);
        glVertex2f(0, dim.y);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    // Draw buttons (ensure their positions are correct)
    for (const auto& button : pauseMenuButtons) {
        drawButton(button);
    }

    // Restore matrices
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void _scene::drawGameOverScreen() {
    string message = isVictory ? "Victory" : "Game Over";
    float textScale = 2.0f * scaleY;
    float textWidth = calculateTextWidth(message, textScale);
    float textX = (dim.x - textWidth) / 2.0f;
    float textY = dim.y * 0.75f;
    renderText(message, textX, textY, textScale, 0.0f);

    for (const auto& button : gameOverButtons) {
        drawButton(button);
    }
}

void _scene::drawHelpScreen() {
    float textScale = 1.0f * scaleY;
    float textX = dim.x * 0.37f;  // 37% of screen width
    float textY = dim.y * 0.8f;   // 80% of screen height
    renderText("Controls", textX, textY, textScale * 3.0f, 0.0f);
    renderText("WASD - Move", textX + 110.0f * scaleX, textY - 200.0f * scaleY, textScale, 0.0f);
    renderText("Mouse - Aim", textX + 110.0f * scaleX, textY - 250.0f * scaleY, textScale, 0.0f);
    renderText("P - Pause", textX + 110.0f * scaleX, textY - 300.0f * scaleY, textScale, 0.0f);

    for (const auto& button : helpScreenButtons) {
        drawButton(button);
    }
}

void _scene::drawCreditsScreen() {
    float textScale = 1.0f * scaleY;
    float textX = dim.x * 0.38f;  // 38% of screen width
    float textY = dim.y * 0.8f;   // 80% of screen height
    renderText("Credits", textX, textY, textScale * 3.0f, 0.0f);
    renderText("Lance Heinrich", textX + 75.0f * scaleX, textY - 200.0f * scaleY, textScale, 0.0f);
    renderText("Brandon Munoz", textX + 75.0f * scaleX, textY - 250.0f * scaleY, textScale, 0.0f);
    renderText("Matias Avila", textX + 75.0f * scaleX, textY - 300.0f * scaleY, textScale, 0.0f);

    for (const auto& button : creditsScreenButtons) {
        drawButton(button);
    }
}

void _scene::drawScene(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (currentState == PLAYING || currentState == PAUSED || currentState == UPGRADE_MENU || currentState == ENDING_SEQUENCE || currentState == GAME_OVER)
    {
        updateMagnet(deltaTime);
        sounds->cleanupSounds();
        vec3 cameraPos = {player->playerPosition.x, player->playerPosition.y, player->playerPosition.z + 20.0f};
        vec3 lookAtPos = {player->playerPosition.x, player->playerPosition.y, player->playerPosition.z};
        gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
                  lookAtPos.x, lookAtPos.y, lookAtPos.z,
                  0, 1, 0);

        glPushMatrix();
            glColor3f(1.0f, 1.0f, 1.0f);
            prlx1->drawBackground(dim.x, dim.y, player->playerPosition);
        glPopMatrix();

        glPushMatrix();
            if(currentState == PLAYING)
            {
                player->drawPlayer();
            }
            else if ((currentState == ENDING_SEQUENCE && isVictory == true) || (currentState == GAME_OVER && isVictory == true))
            {
                player->drawPlayer();
            }
        glPopMatrix();

        if(hasPlayedEndingExplosion)
        {
            if(gameOverExplosion->isActive())
            {
                gameOverExplosion->draw();
                gameOverExplosion->update(deltaTime);
            }
        }

        if (debugMode) {
            vec3 playerMin = player->getCollisionBoxMin();
            vec3 playerMax = player->getCollisionBoxMax();
            glPushMatrix();
                glDisable(GL_TEXTURE_2D);
                glTranslatef(player->playerPosition.x, player->playerPosition.y, player->playerPosition.z + 0.1f);
                glRotatef(player->playerRotation.z, 0, 0, 1);
                glColor3f(0.0f, 0.0f, 1.0f);
                glBegin(GL_LINE_LOOP);
                    glVertex3f(playerMin.x - player->playerPosition.x, playerMin.y - player->playerPosition.y, 0);
                    glVertex3f(playerMax.x - player->playerPosition.x, playerMin.y - player->playerPosition.y, 0);
                    glVertex3f(playerMax.x - player->playerPosition.x, playerMax.y - player->playerPosition.y, 0);
                    glVertex3f(playerMin.x - player->playerPosition.x, playerMax.y - player->playerPosition.y, 0);
                glEnd();
                glColor3f(1.0f, 1.0f, 1.0f);
                glEnable(GL_TEXTURE_2D);
            glPopMatrix();
        }

        // Step 1: Update the enemy grid each frame
        enemyGrid.clear();
        for (auto& enemy : enemies)
        {
            if (enemy.isAlive)
            {
                AABB enemyAABB = enemy.getAABB();
                enemyGrid.addObject(&enemy, enemyAABB);
            }
        }

        // Step 2: Bullet-enemy collisions using the grid
        glPushMatrix();
        if (currentState == PLAYING)
        {
            player->updateWeapons(deltaTime, enemies, worldMousePos, sounds);
            player->bullets.erase(
                std::remove_if(player->bullets.begin(), player->bullets.end(),
                               [](const _Bullet& b) { return !b.isAlive; }),
                player->bullets.end()
            );
        }
            for (auto& bullet : player->bullets) {
                if (bullet.isAlive) {
                    bullet.drawBullet(deltaTime);
                    vec3 bulletMin = bullet.getCollisionBoxMin();
                    vec3 bulletMax = bullet.getCollisionBoxMax();

                    // Get potential collisions from the grid
                    AABB bulletAABB = bullet.getAABB();
                    std::vector<_enemy*> potentialEnemies = enemyGrid.getPotentialCollisions(bulletAABB);

                    if (bullet.weapon.type == LASER) {
                        for (_enemy* enemy : potentialEnemies) {
                            if (enemy->isAlive && collision->isOBBCollision(bullet, *enemy)) {
                                enemy->takeDamage(bullet.weapon.damage * deltaTime, xpOrbs, xpOrbTexture,
                                                  enemyDrops, enemyDropsMagnetTexture, enemyDropsHealthTexture, sounds);
                            }
                        }
                    } else if (bullet.weapon.type != ENERGY_FIELD) {
                        for (_enemy* enemy : potentialEnemies) {
                            size_t enemyIndex = std::distance(enemies.data(), enemy);
                            if (enemy->isAlive &&
                                std::find(bullet.hitEnemies.begin(), bullet.hitEnemies.end(), enemyIndex) ==
                                    bullet.hitEnemies.end()) {
                                if (collision->isOBBCollision(bullet, *enemy)) {
                                    if (bullet.weapon.type != ROCKET)
                                    {
                                        enemy->takeDamage(bullet.weapon.damage, xpOrbs, xpOrbTexture,
                                                          enemyDrops, enemyDropsMagnetTexture, enemyDropsHealthTexture,sounds);
                                    }
                                    bullet.hitEnemies.push_back(enemyIndex);
                                    if (bullet.weapon.type != FLAK)
                                    {
                                        if (bullet.weapon.type == ROCKET) {
                                            if (!bullet.hasExploded) {
                                                bullet.explode(enemies, xpOrbs, xpOrbTexture, enemyDrops,
                                                               enemyDropsMagnetTexture, enemyDropsHealthTexture,sounds);
                                            }
                                        }
                                        if (bullet.weapon.type != ROCKET)
                                        {
                                            bullet.isAlive = false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                if (debugMode) {
                    glPushMatrix();
                        glDisable(GL_TEXTURE_2D);
                        glTranslatef(bullet.position.x, bullet.position.y, bullet.position.z + 0.1f);
                        glRotatef(bullet.rotation.z, 0, 0, 1);
                        glColor3f(0.0f, 1.0f, 0.0f);
                        glBegin(GL_LINE_LOOP);
                            glVertex3f(bulletMin.x - bullet.position.x, bulletMin.y - bullet.position.y, 0);
                            glVertex3f(bulletMax.x - bullet.position.x, bulletMin.y - bullet.position.y, 0);
                            glVertex3f(bulletMax.x - bullet.position.x, bulletMax.y - bullet.position.y, 0);
                            glVertex3f(bulletMin.x - bullet.position.x, bulletMax.y - bullet.position.y, 0);
                        glEnd();
                        glColor3f(1.0f, 1.0f, 1.0f);
                        glEnable(GL_TEXTURE_2D);
                    glPopMatrix();
                }
            }
        }
        glPopMatrix();

        if (currentState == PLAYING) {
            player->playerActions(deltaTime);
            updateEnemySpawning();
        }

        glPushMatrix();
        for (size_t i = 0; i < enemies.size(); i++)
        {
            if (currentState == PLAYING)
            {
                for (size_t j = i + 1; j < enemies.size(); j++) {
                    vec3 diff = {enemies[i].position.x - enemies[j].position.x, enemies[i].position.y - enemies[j].position.y, 0.0f};
                    float distSq = diff.x * diff.x + diff.y * diff.y;
                    float minDist = 0.5f;
                    if (distSq < (minDist * minDist) && distSq > 0.0f) {
                        float dist = sqrt(distSq);
                        float pushForce = (minDist - dist) * 0.05f;
                        diff.x /= dist;
                        diff.y /= dist;
                        enemies[i].position.x += diff.x * pushForce;
                        enemies[i].position.y += diff.y * pushForce;
                        enemies[j].position.x -= diff.x * pushForce;
                        enemies[j].position.y -= diff.y * pushForce;
                    }
                }

                if (enemies[i].isAlive && player->currentHp > 0)
                {
                    enemies[i].playerPosition = player->playerPosition;
                    enemies[i].enemyActions(deltaTime);
                    // Only check player collision if necessary
                    if (i == 0) { // Avoid redundant checks per enemy
                        // Player-enemy collision check (regular enemies)
                        AABB playerAABB = player->getAABB();
                        std::vector<_enemy*> potentialEnemies = enemyGrid.getPotentialCollisions(playerAABB);
                        for (_enemy* enemy : potentialEnemies) {
                            if (enemy->isAlive && !enemy->isBoss && collision->isOBBCollision(*player, *enemy)) {
                                if (damageCooldown <= 0.0f) {
                                    player->takeDamage(10.0f, sounds);
                                    damageCooldown = 0.5f;
                                    if (player->currentHp <= 0) {
                                        currentState = ENDING_SEQUENCE;
                                        isVictory = false;
                                        hasPlayedEndingExplosion = false;
                                        endingTimer = 0.0f;
                                    }
                                }
                            }
                        }
                    }
                    // Separate boss collision check
                    if (bossSpawned) {
                        for (_enemy& enemy : enemies) {
                            if (enemy.isAlive && enemy.isBoss && collision->isOBBCollision(*player, enemy)) {
                                if (damageCooldown <= 0.0f) {
                                    player->takeDamage(20.0f, sounds);
                                    damageCooldown = 0.5f;
                                    if (player->currentHp <= 0) {
                                        currentState = ENDING_SEQUENCE;
                                        isVictory = false;
                                        hasPlayedEndingExplosion = false;
                                        endingTimer = 0.0f;
                                        if (!hasPlayedEndingExplosion) {
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
            enemies[i].drawEnemy(enemies[i].enemyTextureLoader->tex, deltaTime);

            if (debugMode && enemies[i].isAlive) {
                glPushMatrix();
                    glDisable(GL_TEXTURE_2D);
                    glTranslatef(enemies[i].position.x, enemies[i].position.y, enemies[i].position.z + 0.1f);
                    glRotatef(enemies[i].rotation.z, 0, 0, 1);
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glBegin(GL_LINE_LOOP);
                        glVertex3f(enemies[i].getCollisionBoxMin().x - enemies[i].position.x, enemies[i].getCollisionBoxMin().y - enemies[i].position.y, 0);
                        glVertex3f(enemies[i].getCollisionBoxMax().x - enemies[i].position.x, enemies[i].getCollisionBoxMin().y - enemies[i].position.y, 0);
                        glVertex3f(enemies[i].getCollisionBoxMax().x - enemies[i].position.x, enemies[i].getCollisionBoxMax().y - enemies[i].position.y, 0);
                        glVertex3f(enemies[i].getCollisionBoxMin().x - enemies[i].position.x, enemies[i].getCollisionBoxMax().y - enemies[i].position.y, 0);
                    glEnd();
                    glColor3f(1.0f, 1.0f, 1.0f);
                    glEnable(GL_TEXTURE_2D);
                glPopMatrix();
            }
        }
        glPopMatrix();

        if (damageCooldown > 0.0f && currentState == PLAYING)
        {
            damageCooldown -= deltaTime;
            if (damageCooldown < 0.0f) damageCooldown = 0.0f;
        }

        for (auto& orb : xpOrbs)
        {
            if(currentState == PLAYING)
            {
                orb.update(deltaTime, player->playerPosition, xpPickupRange);
            }
            orb.drawOrb();
        }

        if(currentState == PLAYING)
        {
            for (auto& orb : xpOrbs)
            {
                if (orb.isActive && collision->isOBBCollision(*player, orb))
                {
                    orb.isActive = false;
                    if (player->gainXP(1))
                    {
                        showUpgradeMenu();
                    }
                }
            }
        }
        xpOrbs.erase(
            remove_if(xpOrbs.begin(), xpOrbs.end(), [](const _xpOrb& o) { return !o.isActive; }),
            xpOrbs.end()
        );

        for (auto& drop : enemyDrops)
        {
            if(currentState == PLAYING)
            {
                drop.update(deltaTime, player->playerPosition, xpPickupRange, magnetActive, magnetTimer, player->currentHp, player->maxHp, player->hasShield, player->shieldTimer);
            }
            drop.drawDrop();
        }

        enemyDrops.erase(
            remove_if(enemyDrops.begin(), enemyDrops.end(), [](const _enemyDrops& d) { return !d.isActive; }),
            enemyDrops.end()
        );

        if (bossSpawned && !bossDefeated) {
            bool bossAlive = false;
            for (const auto& enemy : enemies) {
                if (enemy.isAlive && enemy.isBoss) {
                    bossAlive = true;
                    break;
                }
            }
            if (!bossAlive) {
                bossDefeated = true;
                currentState = ENDING_SEQUENCE;
                isVictory = true;
                hasPlayedEndingExplosion = false;
                endingTimer = 0.0f;
            }
        }

        // HUD Drawing (Orthographic Projection)
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, dim.x, 0, dim.y);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        scaleX = dim.x / refWidth;
        scaleY = dim.y / refHeight;

        float displayTime = std::min(elapsedTime, 600.0f);
        int minutes = static_cast<int>(displayTime) / 60;
        int seconds = static_cast<int>(displayTime) % 60;
        string timeText = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
        float fontScale = scaleY * 2.0f;
        float textX = (dim.x / 2.0f) - (timeText.length() * 20.0f * fontScale / 2.0f);
        float textY = dim.y - (90.0f * scaleY);
        renderText(timeText, textX, textY, fontScale, 0.0f);

        // Health Bar
        float healthBarWidth = 425.0f * scaleX;
        float healthBarHeight = 80.0f * scaleY;
        float healthBarX = 0.0f;
        float healthBarY = 40.0f * scaleY;
        float healthFilledWidth = (player->currentHp / player->maxHp) * healthBarWidth;
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.937f, 0.541f, 0.196f);
        glBegin(GL_QUADS);
            glVertex2f(healthBarX, healthBarY);
            glVertex2f(healthBarX + healthFilledWidth, healthBarY);
            glVertex2f(healthBarX + healthFilledWidth, healthBarY + healthBarHeight);
            glVertex2f(healthBarX, healthBarY + healthBarHeight);
        glEnd();
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);

        // XP Bar
        float xpBarWidth = 425.0f * scaleX;
        float xpBarHeight = 80.0f * scaleY;
        float xpBarX = dim.x - xpBarWidth;
        float xpBarY = 40.0f * scaleY;
        float xpFilledWidth = ((float)player->experiencePoints / player->xpThresh) * xpBarWidth;
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
            glVertex2f(xpBarX, xpBarY);
            glVertex2f(xpBarX + xpBarWidth, xpBarY);
            glVertex2f(xpBarX + xpBarWidth, xpBarY + xpBarHeight);
            glVertex2f(xpBarX, xpBarY + xpBarHeight);
        glEnd();
        glColor3f(0.573f, 0.878f, 0.914f);
        glBegin(GL_QUADS);
            glVertex2f(xpBarX, xpBarY);
            glVertex2f(xpBarX + xpFilledWidth, xpBarY);
            glVertex2f(xpBarX + xpFilledWidth, xpBarY + xpBarHeight);
            glVertex2f(xpBarX, xpBarY + xpBarHeight);
        glEnd();
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);

        // Weapon Icons
        float baseWeaponIconSize = 200.0f * scaleX;
        float weaponStartX = dim.x - (150.0f * scaleX);
        float weaponStartY = dim.y - (300.0f * scaleY);
        int slot = 0;
        const float rocketPositions[] = { dim.y - (200.0f * scaleY), dim.y - (355.0f * scaleY), dim.y - (470.0f * scaleY), dim.y - (585.0f * scaleY)};
        const float laserPositions[] = { dim.y - (200.0f * scaleY), dim.y - (410.0f * scaleY), dim.y - (520.0f * scaleY), dim.y - (630.0f * scaleY)};
        const float flakPositions[] = { dim.y - (200.0f * scaleY), dim.y - (345.0f * scaleY), dim.y - (457.0f * scaleY), dim.y - (568.0f * scaleY)};

        for (const auto& weapon : player->weapons) {
            if (!weapon.isActive) continue;
            shared_ptr<_textureLoader> weaponTex;
            float iconSize = baseWeaponIconSize;
            float currentY = weaponStartY;
            float currentX = weaponStartX;
            float textOffsetX = 0.0f;
            float textOffsetY = 0.0f;

            switch (weapon.type) {
                case DEFAULT:
                    weaponTex = player->bulletTextureLoader;
                    currentY = weaponStartY;
                    currentX = weaponStartX;
                    textOffsetX = -25.0f * scaleX;
                    textOffsetY = -5.0f * scaleY;
                    break;
                case ROCKET:
                    weaponTex = player->rocketTex;
                    iconSize = baseWeaponIconSize / 2.0f;
                    currentY = rocketPositions[slot];
                    currentX += 50.0f*scaleX;
                    textOffsetX = -75.0f * scaleX;
                    textOffsetY = -10.0f;
                    break;
                case LASER:
                    weaponTex = player->laserTex;
                    currentY = laserPositions[slot];
                    currentX -= 75.0f*scaleX;
                    textOffsetX = 50.0f * scaleX;
                    textOffsetY = -5.0f * scaleY;
                    break;
                case FLAK:
                    weaponTex = player->flakTex;
                    iconSize = baseWeaponIconSize / 3.0f;
                    currentY = flakPositions[slot];
                    currentX += 60.0f*scaleX;
                    textOffsetX = -85.0f * scaleX;
                    textOffsetY = -5.0f;
                    break;
                default:
                    continue;
            }

            float weaponX = currentX;
            float weaponY = currentY;

            if (weaponTex) {
                glPushMatrix();
                weaponTex->textureBinder();
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 1.0f); glVertex2f(weaponX, weaponY);
                    glTexCoord2f(1.0f, 1.0f); glVertex2f(weaponX + iconSize, weaponY);
                    glTexCoord2f(1.0f, 0.0f); glVertex2f(weaponX + iconSize, weaponY + iconSize);
                    glTexCoord2f(0.0f, 0.0f); glVertex2f(weaponX, weaponY + iconSize);
                glEnd();
                glPopMatrix();
            }

            string levelText = "LV" + std::to_string(weapon.level);
            float textX = weaponX + textOffsetX - (12.0f * scaleX);
            float textY = weaponY + (iconSize / 2.0f) + textOffsetY;
            renderText(levelText, textX, textY, scaleY * 0.8f, 0.0f);
            slot++;
        }

        // Upgrade Icons
        float baseUpgradeIconSize = 150.0f * scaleX;
        float upgradeStartX = 0;
        float upgradeStartY = dim.y - (150.0f * scaleY);
        int upgradeSlot = 0;
        const float damagePositions[] = { dim.y - (280.0f * scaleY), dim.y - (395.0f * scaleY), dim.y - (505.0f * scaleY), dim.y - (620.0f * scaleY)};
        const float fireRatePositions[] = { dim.y - (260.0f * scaleY), dim.y - (375.0f * scaleY), dim.y - (485.0f * scaleY), dim.y - (595.0f * scaleY)};
        const float aoeSizePositions[] = { dim.y - (250.0f * scaleY), dim.y - (365.0f * scaleY), dim.y - (475.0f * scaleY), dim.y - (585.0f * scaleY)};
        const float speedPositions[] = { dim.y - (290.0f * scaleY), dim.y - (400.0f * scaleY), dim.y - (510.0f * scaleY), dim.y - (620.0f * scaleY)};
        const float healthPositions[] = { dim.y - (255.0f * scaleY), dim.y - (365.0f * scaleY), dim.y - (480.0f * scaleY), dim.y - (590.0f * scaleY)};

        std::map<std::string, std::shared_ptr<_textureLoader>> upgradeTextures = {
            {"Damage", damageIconTexture},
            {"FireRate", fireRateIconTexture},
            {"AoeSize", aoeSizeIconTexture},
            {"Speed", speedIconTexture},
            {"Health", healthIconTexture}
        };

        for (const auto& upgrade : activeUpgrades) {
            auto texIt = upgradeTextures.find(upgrade.name);
            if (texIt == upgradeTextures.end()) continue;
            std::shared_ptr<_textureLoader> tex = texIt->second;
            float iconSize = baseUpgradeIconSize;
            float currentY = upgradeStartY;
            float currentX = upgradeStartX;

            if (upgrade.name == "Damage") {
                iconSize = baseUpgradeIconSize * 1.0f;
                currentX = -30.0f * scaleX;
                currentY = damagePositions[upgradeSlot];
            } else if (upgrade.name == "FireRate") {
                iconSize = baseUpgradeIconSize * 0.8f;
                currentX = -5.0f * scaleX;
                currentY = fireRatePositions[upgradeSlot];
            } else if (upgrade.name == "AoeSize") {
                iconSize = baseUpgradeIconSize * 0.7f;
                currentX = -5.0f * scaleX;
                currentY = aoeSizePositions[upgradeSlot];
            } else if (upgrade.name == "Speed") {
                iconSize = baseUpgradeIconSize * 1.25f;
                currentX = -50.0f * scaleX;
                currentY = speedPositions[upgradeSlot];
            } else if (upgrade.name == "Health") {
                iconSize = baseUpgradeIconSize * 0.8f;
                currentX = -12.0f * scaleX;
                currentY = healthPositions[upgradeSlot];
            }

            glPushMatrix();
            if (tex) {
                tex->textureBinder();
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 1.0f); glVertex2f(currentX, currentY);
                    glTexCoord2f(1.0f, 1.0f); glVertex2f(currentX + iconSize, currentY);
                    glTexCoord2f(1.0f, 0.0f); glVertex2f(currentX + iconSize, currentY + iconSize);
                    glTexCoord2f(0.0f, 0.0f); glVertex2f(currentX, currentY + iconSize);
                glEnd();
            }
            glPopMatrix();

            string levelText = "LV" + std::to_string(upgrade.level);
            float textX = currentX + iconSize + (12.0f * scaleX);
            float textY = currentY + (iconSize / 2.0f);
            renderText(levelText, textX, textY, scaleY * 0.8f, 0.0f);
            upgradeSlot++;
            if (upgradeSlot >= 5) break;
        }
        if (currentState == GAME_OVER)
        {
            // Use same HUD projection
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            gluOrtho2D(0, dim.x, 0, dim.y);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            drawGameOverScreen();

            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
        }
        if (currentState == ENDING_SEQUENCE)
        {
            endingTimer += deltaTime;

            if (!hasPlayedEndingExplosion)
            {
                if (!isVictory)
                {
                    player->weapons.clear();
                    player->bullets.clear();
                    activeUpgrades.clear();
                    gameOverExplosion->spawnExplosion(player->playerPosition, 100, 2.0f);
                    sounds->play(sounds->rocketExplosionSource);
                    hasPlayedEndingExplosion = true;
                }
                else
                {
                    gameOverExplosion->spawnExplosion(bossEnemy.position, 300, 4.0f);
                    sounds->play(sounds->rocketExplosionSource);
                    hasPlayedEndingExplosion = true;
                }
            }

            if (endingTimer >= endingDelay) {
                currentState = GAME_OVER;
            }
        }
        // Upgrade Menu
        if (currentState == UPGRADE_MENU) {
            glDisable(GL_TEXTURE_2D);
            glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
            glBegin(GL_QUADS);
                glVertex2f(0, 0);
                glVertex2f(dim.x, 0);
                glVertex2f(dim.x, dim.y);
                glVertex2f(0, dim.y);
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f);
            glEnable(GL_TEXTURE_2D);

            float boxWidth = 300.0f * scaleX;
            float boxHeight = 500.0f * scaleY;
            float spacing = 50.0f * scaleX;
            float totalWidth = (3 * boxWidth) + (2 * spacing);
            float startX = (dim.x - totalWidth) / 2.0f;
            float startY = dim.y / 2.0f - (boxHeight / 2.0f);

            for (size_t i = 0; i < currentUpgradeOptions.size() && i < 3; ++i) {
                float x = startX + i * (boxWidth + spacing);
                glDisable(GL_TEXTURE_2D);
                glColor4f(0.5725f, 0.8784f, 0.9137f, 0.5f);
                glBegin(GL_QUADS);
                    glVertex2f(x, startY);
                    glVertex2f(x + boxWidth, startY);
                    glVertex2f(x + boxWidth, startY + boxHeight);
                    glVertex2f(x, startY + boxHeight);
                glEnd();
                glColor3f(1.0f, 1.0f, 1.0f);
                glEnable(GL_TEXTURE_2D);

                float iconSize = 100.0f * scaleX;
                float iconX = x + (boxWidth - iconSize) / 2.0f;
                if (currentUpgradeOptions[i].name == "Weapon_Laser") {
                    iconX -= 30.0f * scaleX;
                }
                float iconY = startY + boxHeight - iconSize - 20.0f * scaleY;
                if (currentUpgradeOptions[i].texture) {
                    glPushMatrix();
                    currentUpgradeOptions[i].texture->textureBinder();
                    glBegin(GL_QUADS);
                        glTexCoord2f(0.0f, 1.0f); glVertex2f(iconX, iconY);
                        glTexCoord2f(1.0f, 1.0f); glVertex2f(iconX + iconSize, iconY);
                        glTexCoord2f(1.0f, 0.0f); glVertex2f(iconX + iconSize, iconY + iconSize);
                        glTexCoord2f(0.0f, 0.0f); glVertex2f(iconX, iconY + iconSize);
                    glEnd();
                    glPopMatrix();
                }

                string nameText = currentUpgradeOptions[i].displayText.substr(0, currentUpgradeOptions[i].displayText.find(" ("));
                string levelText = "Level " + to_string(currentUpgradeOptions[i].currentLevel + 1);
                string descText = currentUpgradeOptions[i].description;

                float textScale = 0.8f * scaleY;
                float textSpacing = 30.0f * scaleY;

                float nameX = x + (boxWidth - calculateTextWidth(nameText, textScale)) / 2.0f;
                float nameY = iconY - 50.0f * scaleY;
                renderText(nameText, nameX, nameY, textScale, 0.0f);

                float levelX = x + (boxWidth - calculateTextWidth(levelText, textScale)) / 2.0f;
                float levelY = nameY - textSpacing - 20.0f * scaleY;
                renderText(levelText, levelX, levelY, textScale, 0.0f);

                float maxTextWidth = boxWidth - 20.0f * scaleX;
                float descY = levelY - textSpacing - 20.0f * scaleY;
                vector<string> lines = {""};
                string currentWord = "";
                for (char c : descText + " ") {
                    if (c == ' ') {
                        if (!currentWord.empty()) {
                            string testLine = lines.back() + (lines.back().empty() ? "" : " ") + currentWord;
                            if (calculateTextWidth(testLine, textScale) <= maxTextWidth) {
                                lines.back() = testLine;
                            } else {
                                lines.push_back(currentWord);
                            }
                            currentWord = "";
                        }
                    } else {
                        currentWord += c;
                    }
                }
                for (const auto& line : lines) {
                    float lineX = x + (boxWidth - calculateTextWidth(line, textScale)) / 2.0f;
                    renderText(line, lineX, descY, textScale, 0.0f);
                    descY -= textSpacing;
                }
            }
        }

        // HUD Texture
        float hudWidth = dim.x;
        float hudHeight = dim.y;
        if (hudTexture) {
            hudTexture->textureBinder();
        }
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(hudWidth, 0.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(hudWidth, hudHeight);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, hudHeight);
        glEnd();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        if (currentState == PAUSED) {
            drawPauseMenu();
        }
    }
    else if (currentState == MAIN_MENU) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, (GLfloat)dim.x / (GLfloat)dim.y, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        vec3 cameraPos = {0.0f, 0.0f, 70.0f};
        vec3 lookAtPos = {0.0f, 0.0f, 50.0f};
        gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
                  lookAtPos.x, lookAtPos.y, lookAtPos.z,
                  0, 1, 0);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, dim.x, 0, dim.y);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(1.0f, 1.0f, 1.0f);
        mainmenu->drawMainMenu(dim.x, dim.y);

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, dim.x, 0, dim.y);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        drawMainMenu();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
    else if (currentState == HELP_SCREEN) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, (GLfloat)dim.x / (GLfloat)dim.y, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        vec3 cameraPos = {0.0f, 0.0f, 70.0f};
        vec3 lookAtPos = {0.0f, 0.0f, 50.0f};
        gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
                  lookAtPos.x, lookAtPos.y, lookAtPos.z,
                  0, 1, 0);

        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        prlx1->drawBackground(dim.x, dim.y, {0.0f, 0.0f, 50.0f});
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, dim.x, 0, dim.y);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        drawHelpScreen();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
    else if (currentState == CREDITS_SCREEN) {

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, (GLfloat)dim.x / (GLfloat)dim.y, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        vec3 cameraPos = {0.0f, 0.0f, 70.0f};
        vec3 lookAtPos = {0.0f, 0.0f, 50.0f};
        gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
                  lookAtPos.x, lookAtPos.y, lookAtPos.z,
                  0, 1, 0);

        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        prlx1->drawBackground(dim.x, dim.y, {0.0f, 0.0f, 50.0f});
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, dim.x, 0, dim.y);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        drawCreditsScreen();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
}

void _scene::showUpgradeMenu() {
    int nonWeaponUpgradeCount = 0;
    int weaponCount = 0;
    for (const auto& upgrade : activeUpgrades) {
        if (upgrade.isWeapon) {
            weaponCount++;
        } else {
            nonWeaponUpgradeCount++;
        }
    }

    vector<string> validUpgrades;
    for (const auto& upgrade : availableUpgrades) {
        bool isWeapon = (upgrade.find("Weapon_") == 0);
        string baseName = isWeapon ? upgrade.substr(7) : upgrade;
        auto it = std::find_if(activeUpgrades.begin(), activeUpgrades.end(),
            [&upgrade](const ActiveUpgrade& up) { return up.name == upgrade; });
        int currentLevel = (it != activeUpgrades.end()) ? it->level : 0;

        if (currentLevel >= MAX_UPGRADE_LEVEL) {
            continue;
        }

        if (!isWeapon) {
            if (nonWeaponUpgradeCount < MAX_NON_WEAPON_UPGRADES || currentLevel > 0) {
                validUpgrades.push_back(upgrade);
            }
        } else {
            if (baseName == "Default" && currentLevel > 0 && currentLevel < MAX_UPGRADE_LEVEL) {
                validUpgrades.push_back(upgrade);
                continue;
            }
            if (weaponCount < MAX_WEAPONS || currentLevel > 0) {
                validUpgrades.push_back(upgrade);
            }
        }
    }

    if (validUpgrades.empty()) {
        upgradeMenuActive = false;
        isPaused = false;
        currentUpgradeOptions.clear();
        return;
    }

    // Map internal names to display names
    std::map<std::string, std::string> displayNameMap = {
        {"Weapon_Default", "Laser Cannon"},
        {"Weapon_Rocket", "Rocket Turret"},
        {"Weapon_Laser", "Laser"},
        {"Weapon_Flak", "Flak Gun"},
        {"Damage", "Damage Boost"},
        {"FireRate", "Fire Rate"},
        {"AoeSize", "Area Size"},
        {"Health", "Health"},
        {"Speed", "Speed"}
    };

    // Map upgrades to their textures (use shared_ptr)
    std::map<std::string, std::shared_ptr<_textureLoader>> upgradeTextures = {
        {"Weapon_Default", _textureManager::getInstance().getTexture("bullet")},
        {"Weapon_Rocket", _textureManager::getInstance().getTexture("rocket")},
        {"Weapon_Laser", _textureManager::getInstance().getTexture("laser")},
        {"Weapon_Flak", _textureManager::getInstance().getTexture("flak")},
        {"Damage", _textureManager::getInstance().getTexture("damage")},
        {"FireRate", _textureManager::getInstance().getTexture("fireRate")},
        {"AoeSize", _textureManager::getInstance().getTexture("aoeSize")},
        {"Speed", _textureManager::getInstance().getTexture("speed")},
        {"Health", _textureManager::getInstance().getTexture("health")}
    };

    upgradeMenuActive = true;
    isPaused = true;
    currentState = UPGRADE_MENU;
    currentUpgradeOptions.clear();
    random_shuffle(validUpgrades.begin(), validUpgrades.end());
    int optionsToSelect = min(3, (int)validUpgrades.size());

    for (int i = 0; i < optionsToSelect; ++i) {
        UpgradeOption option;
        option.name = validUpgrades[i];
        option.isWeapon = (option.name.find("Weapon_") == 0);

        // Set display name
        auto displayIt = displayNameMap.find(option.name);
        option.displayText = (displayIt != displayNameMap.end()) ? displayIt->second : option.name;

        // Set texture
        auto texIt = upgradeTextures.find(option.name);
        option.texture = (texIt != upgradeTextures.end()) ? texIt->second : nullptr;

        if (option.isWeapon) {
            string weaponName = option.name.substr(7);
            if (weaponName == "Default") option.weaponType = DEFAULT;
            else if (weaponName == "Rocket") option.weaponType = ROCKET;
            else if (weaponName == "Laser") option.weaponType = LASER;
            else if (weaponName == "Flak") option.weaponType = FLAK;

            auto it = std::find_if(player->weapons.begin(), player->weapons.end(),
                [&](const Weapon& w) { return w.type == option.weaponType; });
            option.currentLevel = (it != player->weapons.end()) ? it->level : 0;
        } else {
            if (option.name == "Damage") option.currentLevel = (int)((player->damageMultiplier - 1.0f) / 0.1f);
            else if (option.name == "Speed") option.currentLevel = (int)((player->speedMultiplier - 1.0f) / 0.1f);
            else if (option.name == "Health") option.currentLevel = (int)((player->healthMultiplier - 1.0f) / 0.1f);
            else if (option.name == "FireRate") option.currentLevel = (int)((1.0f - player->fireRateMultiplier) / 0.1f);
            else if (option.name == "AoeSize") option.currentLevel = (int)((player->aoeSizeMultiplier - 1.0f) / 0.1f);
        }

        // Set display text and description
        option.displayText = option.displayText + " (Level " + std::to_string(option.currentLevel + 1) + ")";
        if (!option.isWeapon) {
            if (option.name == "Damage") option.description = "Increases damage by 10%";
            if (option.name == "Speed") option.description = "Increases movement speed by 10%";
            if (option.name == "Health") option.description = "Increases max HP by 10%";
            if (option.name == "FireRate") option.description = "Increases fire rate by 10%";
            if (option.name == "AoeSize") option.description = "Increases area of effect by 10%";
        } else {
            if (option.name == "Weapon_Default") option.description = "Fires dual laser cannons";
            if (option.name == "Weapon_Rocket") option.description = "Launches explosive rockets";
            if (option.name == "Weapon_Laser") option.description = "Emits a continuous laser beam";
            if (option.name == "Weapon_Flak") option.description = "Sprays shrapnel in a wide arc";
        }

        currentUpgradeOptions.push_back(option);
    }
}

void _scene::selectUpgrade(int choice) {
    if (choice >= 0 && choice < (int)currentUpgradeOptions.size()) {
        UpgradeOption& selected = currentUpgradeOptions[choice];
        if (selected.isWeapon) {
            player->applyWeaponUpgrade(selected.weaponType);
        } else {
            player->applyUpgrade(selected.name);
        }
        auto it = std::find_if(activeUpgrades.begin(), activeUpgrades.end(),
            [&selected](const ActiveUpgrade& up) { return up.name == selected.name; });
        if (it != activeUpgrades.end()) {
            if (it->level < MAX_UPGRADE_LEVEL) {
                it->level = selected.currentLevel + 1;
            }
        } else {
            activeUpgrades.push_back({selected.name, selected.currentLevel + 1, selected.isWeapon});
        }
    }
    upgradeMenuActive = false;
    isPaused = false;
    currentState = PLAYING;
    currentUpgradeOptions.clear();
}

void _scene::reSize(GLint width, GLint height){
    dim.x = width;
    dim.y = height;

    scaleX = dim.x / refWidth;
    scaleY = dim.y / refHeight;

    GLfloat aspectRatio = (GLfloat)width/(GLfloat)height;
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,aspectRatio,0.1,100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    initMenus();
}

void _scene::processKeyboardInput() {
    gameTime += deltaTime;

    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(GetActiveWindow(), &mousePos);

    // Calculate worldMousePos for PLAYING state
    if (currentState == PLAYING) {
        worldMousePos.x = (mousePos.x - dim.x / 2) / (float)(dim.x / 2);
        worldMousePos.y = (dim.y / 2 - mousePos.y) / (float)(dim.y / 2);
        worldMousePos.z = 0.0f;
    }

    // Check current mouse button state
    bool isMousePressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    // Handle mouse clicks for menus only on new press (transition from not pressed to pressed)
    float mx = mousePos.x;
    float my = dim.y - mousePos.y; // Flip y for OpenGL
    if (isMousePressed && !wasMousePressed) {
        if (currentState == MAIN_MENU) {
            for (auto& button : mainMenuButtons) {
                if (mx >= button.x && mx <= button.x + button.width &&
                    my >= button.y && my <= button.y + button.height) {
                    button.onClick();
                    break;
                }
            }
        } else if (currentState == PAUSED) {
            for (auto& button : pauseMenuButtons) {
                if (mx >= button.x && mx <= button.x + button.width &&
                    my >= button.y && my <= button.y + button.height) {
                    button.onClick();
                    break;
                }
            }
        } else if (currentState == GAME_OVER) {
            for (auto& button : gameOverButtons) {
                if (mx >= button.x && mx <= button.x + button.width &&
                    my >= button.y && my <= button.y + button.height) {
                    button.onClick();
                    break;
                }
            }
        } else if (currentState == HELP_SCREEN) {
            for (auto& button : helpScreenButtons) {
                if (mx >= button.x && mx <= button.x + button.width &&
                    my >= button.y && my <= button.y + button.height) {
                    button.onClick();
                    break;
                }
            }
        } else if (currentState == CREDITS_SCREEN) {
            for (auto& button : creditsScreenButtons) {
                if (mx >= button.x && mx <= button.x + button.width &&
                    my >= button.y && my <= button.y + button.height) {
                    button.onClick();
                    break;
                }
            }
        } else if (currentState == UPGRADE_MENU) {
            float boxWidth = 300.0f * scaleX;
            float boxHeight = 500.0f * scaleY;
            float spacing = 50.0f * scaleX;
            float totalWidth = (3 * boxWidth) + (2 * spacing);
            float startX = (dim.x - totalWidth) / 2.0f;
            float startY = dim.y / 2.0f - (boxHeight / 2.0f);
            for (int i = 0; i < 3; ++i) {
                float x = startX + i * (boxWidth + spacing);
                if (mx >= x && mx <= x + boxWidth && my >= startY && my <= startY + boxHeight) {
                    selectUpgrade(i);
                    break;
                }
            }
        }
    }

    // Update wasMousePressed for the next frame
    wasMousePressed = isMousePressed;

    // Handle keyboard input for PLAYING state
    if (currentState == PLAYING && !isPaused) {
        input->updateMouseRotation(player, mousePos.x, mousePos.y, dim.x, dim.y);
        input->keyPressed(player, sounds, deltaTime);
        input->keyUp(player, sounds);
    }

    // Handle pause key
    static bool lastPauseState = false;
    if (currentState == PLAYING && GetAsyncKeyState('P') & 0x8000) {
        bool currentStateKey = true;
        if (!lastPauseState && currentStateKey) {
            currentState = PAUSED;
        }
        lastPauseState = currentStateKey;
    } else {
        lastPauseState = false;
    }

    // Handle debug mode toggle
    static bool lastDebugState = false;
    if (GetAsyncKeyState('F') & 0x8000) {
        bool currentStateKey = true;
        if (!lastDebugState && currentStateKey) {
            debugMode = !debugMode;
        }
        lastDebugState = currentStateKey;
    } else {
        lastDebugState = false;
    }
}

void _scene::updateEnemySpawning()
{
    if (currentState != PLAYING || bossSpawned) return;

    // Clear the screen at 290 seconds
    if (elapsedTime >= 297.0f && !clearScreenDone) {
        clearScreen();
        clearScreenDone = true;
    }

    // Prevent spawning between 290 and 300 seconds
    if (clearScreenDone && elapsedTime < 300.0f) {
        return;
    }

    // Only allow regular enemy spawning before 300 seconds
    if (elapsedTime < 300.0f) {
        float timeFactor = elapsedTime / 300.0f;
        spawnInterval = 2.0f * exp(-3.0f * timeFactor) + 0.05f;
        spawnInterval = std::max(0.05f, spawnInterval);

        int maxEnemies = 50 + static_cast<int>(timeFactor * 450);
        maxEnemies = std::min(maxEnemies, 500);

        if (elapsedTime - lastSpawnTime >= spawnInterval && enemies.size() < static_cast<size_t>(maxEnemies)) {
            int batchSize = 5 + static_cast<int>(timeFactor * 25);
            batchSize = std::min(batchSize, 30);

            for (int i = 0; i < batchSize; i++) {
                vec3 randPos;
                float angle = (rand() % 360) * (M_PI / 180.0f);
                float distance = 15.0f + (rand() % 11) * 1.0f;
                randPos.x = player->playerPosition.x + cos(angle) * distance;
                randPos.y = player->playerPosition.y + sin(angle) * distance;
                randPos.z = 48.0f;

                float spawnChance = (rand() % 100) / 100.0f;
                bool spawnBasic = (elapsedTime < 60.0f) || (elapsedTime < 150.0f && spawnChance < 0.6f) || (elapsedTime >= 150.0f && spawnChance < 0.4f);
                bool spawnBugShip = elapsedTime >= 150.0f && spawnChance >= 0.4f && spawnChance < 0.8f;

                bool reused = false;
                for (auto& enemy : enemies) {
                    if (!enemy.isAlive) {
                        enemy.placeEnemy(randPos);
                        enemy.isAlive = true;
                        if (spawnBasic) {
                            enemy.initEnemy(SWARMBOT, 15.0f * getEnemyHPScalingFactor(), {0.25f, 0.25f, 1.0f}, 2.0f);
                        } else if (spawnBugShip) {
                            enemy.initEnemy(BUGSHIP, 25.0f * getEnemyHPScalingFactor(), {0.8f, 0.8f, 1.0f}, 1.5f);
                        } else {
                            enemy.initEnemy(SWARMBOT, 15.0f * getEnemyHPScalingFactor(), {0.25f, 0.25f, 1.0f}, 2.0f);
                        }
                        enemy.scale = {1.0f, 1.0f};
                        enemy.playerPosition = player->playerPosition;
                        enemy.currentHp = enemy.maxHp;
                        reused = true;
                        break;
                    }
                }
                if (!reused && enemies.size() < static_cast<size_t>(maxEnemies)) {
                    _enemy newEnemy;
                    newEnemy.placeEnemy(randPos);
                    newEnemy.isAlive = true;
                    if (spawnBasic) {
                        newEnemy.initEnemy(SWARMBOT, 15.0f * getEnemyHPScalingFactor(), {0.25f, 0.25f, 1.0f}, 2.0f);
                    } else if (spawnBugShip) {
                        newEnemy.initEnemy(BUGSHIP, 25.0f * getEnemyHPScalingFactor(), {0.8f, 0.8f, 1.0f}, 1.5f);
                    } else {
                        newEnemy.initEnemy(SWARMBOT, 15.0f * getEnemyHPScalingFactor(), {0.25f, 0.25f, 1.0f}, 2.0f);
                    }
                    newEnemy.scale = {1.0f, 1.0f};
                    newEnemy.playerPosition = player->playerPosition;
                    newEnemy.currentHp = newEnemy.maxHp;
                    enemies.push_back(newEnemy);
                }
            }
            lastSpawnTime = elapsedTime;
        }

        if (elapsedTime >= 60.0f && elapsedTime - lastBugSpawnTime >= bugSpawnInterval) {
            spawnBugSwarm();
            lastBugSpawnTime = elapsedTime;
            bugSpawnInterval = 15.0f * exp(-2.0f * timeFactor) + 5.0f;
        }
    }

    // Boss spawning at 300 seconds
    if (elapsedTime >= 300.0f && !bossSpawned) {
        vec3 bossPos = player->playerPosition;
        bossPos.x += 10.0f;
        bossPos.z = 48.0f;
        bossEnemy.placeEnemy(bossPos);
        bossEnemy.isAlive = true;
        bossEnemy.scale = {3.0f, 3.0f};
        bossEnemy.playerPosition = player->playerPosition;
        bossEnemy.currentHp = bossEnemy.maxHp;
        bossSpawned = true;
        bossEnemy.isBoss = true;
        enemies.push_back(bossEnemy);
    }
}

void _scene::spawnBugSwarm() {
    int maxEnemies = 50 + static_cast<int>(elapsedTime / 300.0f * 450); // Changed from 600.0f to 300.0f
    maxEnemies = std::min(maxEnemies, 500);

    if (enemies.size() >= static_cast<size_t>(maxEnemies)) return;

    float angle = (rand() % 360) * (M_PI / 180.0f);
    float distance = 15.0f + (rand() % 11) * 1.0f;
    vec3 centerPos;
    centerPos.x = player->playerPosition.x + cos(angle) * distance;
    centerPos.y = player->playerPosition.y + sin(angle) * distance;
    centerPos.z = 48.0f;

    int batchSize = 20 + static_cast<int>(elapsedTime / 300.0f * 80); // Changed from 600.0f to 300.0f
    batchSize = std::min(batchSize, 100);
    float swarmRadius = 3.0f;

    for (int i = 0; i < batchSize; i++) {
        float offsetAngle = (rand() % 360) * (M_PI / 180.0f);
        float offsetDist = (rand() % 100) / 100.0f * swarmRadius;
        vec3 randPos;
        randPos.x = centerPos.x + cos(offsetAngle) * offsetDist;
        randPos.y = centerPos.y + sin(offsetAngle) * offsetDist;
        randPos.z = centerPos.z;

        bool reused = false;
        for (auto& enemy : enemies) {
            if (!enemy.isAlive) {
                enemy.placeEnemy(randPos);
                enemy.isAlive = true;
                enemy.initEnemy(SMALLBUG, 10.0f * getEnemyHPScalingFactor(), {0.3f, 0.3f, 1.0f}, 4.0f);
                enemy.scale = {1.0f, 1.0f};
                enemy.playerPosition = player->playerPosition;
                enemy.currentHp = enemy.maxHp;
                reused = true;
                break;
            }
        }
        if (!reused && enemies.size() < static_cast<size_t>(maxEnemies)) {
            _enemy newEnemy;
            newEnemy.placeEnemy(randPos);
            newEnemy.isAlive = true;
            newEnemy.initEnemy(SMALLBUG, 10.0f * getEnemyHPScalingFactor(), {0.3f, 0.3f, 1.0f}, 4.0f);
            newEnemy.scale = {1.0f, 1.0f};
            newEnemy.playerPosition = player->playerPosition;
            enemies.push_back(newEnemy);
        }
    }
}

void _scene::updateDeltaTime() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    static bool wasPausedLastFrame = false;
    if ((currentState == PLAYING && !isPaused) || currentState == ENDING_SEQUENCE) {
        if (wasPausedLastFrame) {
            lastTime = currentTime;
            deltaTime = 0.0f;
            wasPausedLastFrame = false;
        } else {
            deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
            lastTime = currentTime;
            if (elapsedTime < 600.0f) { // Cap at 10 minutes
                elapsedTime += deltaTime;
            }
        }
    } else {
        deltaTime = 0.0f;
        wasPausedLastFrame = true;
    }
}
void _scene::renderText(string text, float x, float y, float scale, float spacing){
    if (!fontTexture || fontTexture->tex == 0) {
        std::cerr << "Font texture is null or invalid!" << std::endl;
        return;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    fontTexture->textureBinder();
    glColor3f(1.0f, 1.0f, 1.0f);

    float currentX = x;
    for (char c : text) {
        int ascii = static_cast<unsigned char>(c);
        if (bitmapFont.find(ascii) == bitmapFont.end()) {
            std::cerr << "Character '" << c << "' (ASCII " << ascii << ") not found in font!" << std::endl;
            continue;
        }

        FontChar& fc = bitmapFont[ascii];
        float w = fc.width * scale;
        float h = fc.height * scale;
        float xoff = fc.xoffset * scale;
        // Ignore yoffset to align all characters to the input y
        float u0 = fc.u0;
        float v0 = fc.v0;
        float u1 = fc.u1;
        float v1 = fc.v1;

        glBegin(GL_QUADS);
            glTexCoord2f(u0, v1); glVertex2f(currentX + xoff,     y);
            glTexCoord2f(u1, v1); glVertex2f(currentX + xoff + w, y);
            glTexCoord2f(u1, v0); glVertex2f(currentX + xoff + w, y + h);
            glTexCoord2f(u0, v0); glVertex2f(currentX + xoff,     y + h);
        glEnd();

        currentX += fc.xadvance * scale + spacing;
    }
}

void _scene::loadFontData(const string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open font file: " << path << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("char id=") == std::string::npos) continue;

        FontChar fc;
        sscanf(line.c_str(),
            "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d",
            &fc.id, &fc.x, &fc.y, &fc.width, &fc.height,
            &fc.xoffset, &fc.yoffset, &fc.xadvance);

        float texWidth = 256.0f;
        float texHeight = 256.0f;
        fc.u0 = fc.x / texWidth;
        fc.v0 = fc.y / texHeight;
        fc.u1 = (fc.x + fc.width) / texWidth;
        fc.v1 = (fc.y + fc.height) / texHeight;
        bitmapFont[fc.id] = fc;
    }
}

void _scene::updateMagnet(float deltaTime){
    if (magnetActive)
    {
        magnetTimer -= deltaTime;
        if (magnetTimer <= 0.0f)
        {
            magnetActive = false;
            xpPickupRange = 5.0f; // Reset to default
        }
    }
}

float _scene::getEnemyHPScalingFactor() const
{
    float timeFactor = elapsedTime / 300.0f; // Changed from 600.0f to 300.0f
    if (elapsedTime < 60.0f) { // Changed from 120.0f to 60.0f
        return 1.0f;
    } else {
        float adjustedTimeFactor = (elapsedTime - 60.0f) / 240.0f; // Adjusted from 120.0f/480.0f to 60.0f/240.0f
        return 1.0f + 4.0f * adjustedTimeFactor;
    }
}

void _scene::clearScreen() {
    enemies.clear();
    xpOrbs.clear();
    enemyDrops.clear();
}
