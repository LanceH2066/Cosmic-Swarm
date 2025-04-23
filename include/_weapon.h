#ifndef _WEAPON_H
#define _WEAPON_H

#include "_common.h"
#include "_textureLoader.h"
#include "_timer.h"
#include "_enemy.h"

struct Weapon
{
    WeaponType type;
    int level = 1;               // Weapon level (1-5)
    float damage = 5.0f;         // Base damage
    float fireRate = 1.0f;       // Seconds between shots
    float aoeSize = 1.0f;        // AoE radius or spread angle
    float projSpeed = 30.0f;
    _timer timer;                // Cooldown timer
    shared_ptr<_textureLoader> texture;     // Texture for projectiles/effects
    bool isActive = false;       // Whether the weapon is unlocked

    float baseDamage;
    float baseFireRate;
    float baseAoeSize;
    float baseProjSpeed;

    // Initialize weapon
    void init(WeaponType t, shared_ptr<_textureLoader> tex, float dmg, float rate, float aoe, float speed) {
        type = t;
        texture = tex;
        baseDamage = damage = dmg;
        baseFireRate = fireRate = rate;
        baseAoeSize = aoeSize = aoe;
        isActive = true;
        baseProjSpeed = projSpeed = speed;
        timer.reset();
    }

    void levelUp()
    {
        level++;
        baseDamage += baseDamage*0.25;
        damage = baseDamage;

        baseFireRate -= baseFireRate*0.1;
        fireRate = baseFireRate;

        baseAoeSize += baseAoeSize*0.25;
        aoeSize = baseAoeSize;

        baseProjSpeed += baseProjSpeed*0.1;
        projSpeed = baseProjSpeed;
    }

    void applyMods(float damageMultiplier, float fireRateMultiplier, float aoeSizeMultiplier)
    {
        damage = baseDamage * damageMultiplier;
        fireRate = baseFireRate * fireRateMultiplier;
        aoeSize = baseAoeSize * aoeSizeMultiplier;
    }

    // Check if weapon can fire
    bool canFire()
    {
        return isActive && timer.getTicks() >= (fireRate * 1000.0f); // Convert fireRate to milliseconds
    }
};

#endif // _WEAPON_H
