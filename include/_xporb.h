#ifndef _XPORB_H
#define _XPORB_H

#include<_common.h>
#include<_textureLoader.h>


class _xpOrb
{
    public:
        _xpOrb();
        virtual ~_xpOrb();

        //orb position, size and state
        vec3 position;
        vec3 scale;
        bool isActive;

        //animation/texture
        float xMin, xMax, yMin, yMax;
        std::shared_ptr<_textureLoader> xpTextureLoader;

        //methods
        void initOrb();
        void drawOrb();
        void placeOrb(vec3);

        vec3 getCollisionBoxMin() const;
        vec3 getCollisionBoxMax() const;
        vec3 collisionBoxSize = {0.4f, 0.4f, 1.0f}; // pickup radius

        float speed = 2.0f; // Speed toward player when in range (adjustable)
        void update(float deltaTime, vec3 playerPos, float pickupRange);



    protected:

    private:
};

#endif // _XPORB_H
