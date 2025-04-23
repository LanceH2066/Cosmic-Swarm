#ifndef _TEXTURE_MANAGER_H
#define _TEXTURE_MANAGER_H

#include <_textureLoader.h>
#include <map>
#include <string>
#include <memory>

class _textureManager {
public:
    static _textureManager& getInstance() {
        static _textureManager instance;
        return instance;
    }

    void preloadTextures() {
        // Preload all textures used in the game
        loadTexture("player", "images/spritesheet.png");
        loadTexture("bullet", "images/Greenlasercannon.png");
        loadTexture("rocket", "images/rocket.png");
        loadTexture("laser", "images/laser.png");
        loadTexture("flak", "images/flak.png");
        loadTexture("xpOrb", "images/xpOrb.png");
        loadTexture("magnet", "images/magnet.png");
        loadTexture("healthDrop", "images/healthDrop.png");
        loadTexture("hud", "images/hud.png");
        loadTexture("font", "images/font.png");
        loadTexture("damage", "images/damage.png");
        loadTexture("fireRate", "images/fireRate.png");
        loadTexture("aoeSize", "images/aoeSize.png");
        loadTexture("speed", "images/speed.png");
        loadTexture("health", "images/healthDrop.png");
        loadTexture("background", "images/background.png");
        loadTexture("swarmbot", "images/swarmbot.png");
        loadTexture("bugShip", "images/bugShip.png");
        loadTexture("smallbug", "images/smallbug.png");
        loadTexture("bossShip", "images/bossShip.png");
        loadTexture("particle", "images/particle.png");
        loadTexture("rocketParticle", "images/rocketParticle.png");
    }

    std::shared_ptr<_textureLoader> getTexture(const std::string& key) {
        auto it = textures.find(key);
        if (it != textures.end()) {
            return it->second;
        }
        return nullptr;
    }

private:
    _textureManager() {}
    _textureManager(const _textureManager&) = delete;
    _textureManager& operator=(const _textureManager&) = delete;

    void loadTexture(const std::string& key, const std::string& fileName) {
        auto texture = std::make_shared<_textureLoader>();
        texture->loadTexture(fileName.c_str());
        if (texture->tex == 0) {
            std::cerr << "Failed to load texture: " << fileName << std::endl;
        }
        textures[key] = texture;
    }

    std::map<std::string, std::shared_ptr<_textureLoader>> textures;
};

#endif // _TEXTURE_MANAGER_H
