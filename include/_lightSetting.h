#ifndef _LIGHTSETTING_H
#define _LIGHTSETTING_H

#include<_common.h>

class _lightSetting
{
    public:
        _lightSetting();
        virtual ~_lightSetting();

        const GLfloat light_ambient[4]  = { 0.0f, 0.0f, 0.0f, 1.0f };    // Environment Light
        const GLfloat light_diffuse[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };    // Shadow Light
        const GLfloat light_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };    // Shininess
        const GLfloat light_position[4] = { 2.0f, 5.0f, 5.0f, 0.0f };    // Light Position

        const GLfloat mat_ambient[4]    = { 0.7f, 0.7f, 0.7f, 1.0f };
        const GLfloat mat_diffuse[4]    = { 0.8f, 0.8f, 0.8f, 1.0f };
        const GLfloat mat_specular[4]   = { 1.0f, 1.0f, 1.0f, 1.0f };
        const GLfloat high_shininess[1] = { 100.0f };

        GLvoid setLight(GLenum light);

    protected:

    private:
};

#endif // _LIGHTSETTING_H
