#ifndef _COLLISION_H
#define _COLLISION_H

#include<_common.h>
#include<_Bullet.h>
#include<_enemy.h>
#include<_xporb.h>
#include<_player.h>

class _collision
{
    public:
        _collision();
        virtual ~_collision();

        bool isOBBCollision(const _Bullet& bullet, const _enemy& enemy);
        bool isOBBCollision(const _player& player, const _enemy& enemy);  // Add this for player-enemy collision
        bool isOBBCollision(const _player& player, const _xpOrb& orb);  //player/xp orb collision

    protected:

    private:
};

#endif // _COLLISION_H
