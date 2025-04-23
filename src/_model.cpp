#include "_model.h"

_model::_model()
{
    // Initialization
    rotation.x = rotation.y = rotation.z = 0.0;
    pos.x = pos.y;
    pos.z = -8.0;
    scale.x = scale.y = scale.z = 1.0;
}

_model::~_model()
{

}

void _model::initModel(char* fileName)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    myTex->loadTexture(fileName);
}

void _model::drawModel()
{
    glPushMatrix();
        myTex->textureBinder();
        glTranslatef(pos.x,pos.y,pos.z);    // Set Position
        glRotatef(rotation.x,1,0,0);        // Set Rotation X
        glRotatef(rotation.y,0,1,0);        // Set Rotation Y
        glRotatef(rotation.z,0,0,1);        // Set Rotation Z
        glScalef(scale.x,scale.y,scale.z);  // Set Scale
        glutSolidTeapot(2.0);               // Draw Shape
    glPopMatrix();
}
