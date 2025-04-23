#ifndef _MODEL_H
#define _MODEL_H

#include<_common.h>
#include<_textureLoader.h>

class _model
{
    public:
        _model();
        virtual ~_model();


        vec3 rotation; // model rotation value
        vec3 pos; // model position value
        vec3 scale;    // model scale value

        _textureLoader *myTex = new _textureLoader();

        void drawModel();
        void initModel(char * fileName);

    protected:

    private:
};

#endif // _MODEL_H
