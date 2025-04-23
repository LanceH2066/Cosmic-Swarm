#ifndef _PARALLAX_H
#define _PARALLAX_H

#include<_common.h>
#include<_textureLoader.h>
#include<_timer.h>

class _parallax
{
    public:
        _parallax();
        virtual ~_parallax();

        _textureLoader *background = new _textureLoader();
        _timer *tmr = new _timer();

        void drawBackground(float,float,vec3);   // pass width and height
        void initParallax(char*,float,bool,bool);           // File Name
        void scroll(string);     // Auto move, direction, speed
        void autoScroll();

        float xMax, xMin, yMax, yMin;       // Texture coordinates
        float speed;
        bool upDown, leftRight;

    protected:

    private:
};

#endif // _PARALLAX_H
