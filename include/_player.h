#ifndef _PLAYER_H
#define _PLAYER_H

#include<_common.h>
#include<_Bullet.h>
#include <_sounds.h>
#include<_textureManager.h>

class _player
{
    public:
        _player();
        virtual ~_player();

        shared_ptr<_textureLoader> bulletTextureLoader;
        shared_ptr<_textureLoader> playerTextureLoader;
        shared_ptr<_textureLoader> rocketTex;
        shared_ptr<_textureLoader> laserTex;
        shared_ptr<_textureLoader> flakTex;
        shared_ptr<_textureLoader> energyTex;
        _timer * playerTimer = new _timer();

        void initPlayer(int,int,char*); // number of x,y frames, filename
        void drawPlayer();              // render sprites
        void playerActions(float deltaTime);

        vector<Weapon> weapons; // List of active weapons
        void updateWeapons(float deltaTime, vector<_enemy>& enemies, vec3 mousePos, _sounds* sounds);
        vector<_Bullet> bullets;  // Store active bullets
        void shoot(vec3 mousePos,_sounds *sounds);

        enum {IDLE,FLYING,SHOOTING}; // Player Actions based on sprite
        vec3 playerPosition;
        vec3 playerScale;
        vec3 playerRotation;

        int framesX;    //  number of frame columns
        int framesY;    // number of frame rows
        int currentFrame; // Keeps track of the current animation frame

        float xMax, yMax, xMin, yMin;   // texture coordinates
        vec3 vertices[4];   // draw QUAD to place player sprite

        int actionTrigger;  // select action
        float speed;

        // Add health and damage mechanics
        float maxHp, currentHp;
        void takeDamage(float damage);

        // Add collision box for OBB collision
        vec3 collisionBoxSize = {0.35f, 0.35f, 1.0f};  // Half-extents of the collision box (adjust as needed)
        vec3 getCollisionBoxMin() const { return {playerPosition.x - collisionBoxSize.x, playerPosition.y - collisionBoxSize.y, playerPosition.z - collisionBoxSize.z}; }
        vec3 getCollisionBoxMax() const { return {playerPosition.x + collisionBoxSize.x, playerPosition.y + collisionBoxSize.y, playerPosition.z + collisionBoxSize.z}; }
        vector<vec3> getRotatedCorners() const;
        _enemy* findMostClusteredEnemy(vector<_enemy>& enemies, float clusterRadius);

        //xp orbs
        int experiencePoints = 0;
        int xpThresh;       ////////// new
        int playerLevel;    ///////// new
        bool gainXP(int);
        float xpPickupRange = 5.0f; // Modifiable pickup range (world units)
        bool magnetActive = false;

        bool startFlash = false;
        float flashDuration = 0.2f;  // Duration of flash in seconds
        float flashTimer = 0.0f;     // Tracks elapsed flash time

        // Upgrade modifiers
        float damageMultiplier = 1.0f;  // Multiplies weapon.damage
        float speedMultiplier = 1.0f;   // Multiplies player.speed
        float healthMultiplier = 1.0f;  // Multiplies maxHp
        float fireRateMultiplier = 1.0f; // Multiplies (reduces) fireRate
        float aoeSizeMultiplier = 1.0f; // Multiplies aoeSize

        // Upgrade method
        void applyUpgrade(const string& upgradeType);
        void applyWeaponUpgrade(WeaponType type); // New method for weapon upgrades

        AABB getAABB() const;

    protected:

    private:
};

#endif // _PLAYER_H
