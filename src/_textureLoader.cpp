#include "_textureLoader.h"

_textureLoader::_textureLoader()
{

}

_textureLoader::~_textureLoader()
{
    if (tex != 0) {
        glDeleteTextures(1, &tex);
        tex = 0;
    }
}

void _textureLoader::loadTexture(const char * fileName)
{
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    image = SOIL_load_image(fileName,&width,&height,0,SOIL_LOAD_RGBA);

    if(!image)cout<< "FAILED TO LOAD IMAGE!" << endl;

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
}

void _textureLoader::textureBinder()
{
    glBindTexture(GL_TEXTURE_2D, tex);
}
