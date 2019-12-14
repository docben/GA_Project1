//
// Created by bpiranda on 13/12/2019.
//

#ifndef GA_PROJECT1_ICON_H
#define GA_PROJECT1_ICON_H
#include <glutWindow.h>
#include <string>
#include "Vector2D.h"

class Icon {
    GLuint textureId;
    float width,height;
    string title;
    public:
    Icon(float c_width,float c_height,const string &c_title);
    ~Icon();
    void initTexture();
    void glDraw();
    inline float getWidth() { return width; };
    inline float getHeight() { return height; };
};


#endif //GA_PROJECT1_ICON_H
