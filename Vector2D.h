//
// Created by bpiranda on 22/11/2019.
//

#ifndef GEOMETRY_TRIANGULATION_GL_VECTOR2D_H
#define GEOMETRY_TRIANGULATION_GL_VECTOR2D_H

#include <iostream>
#include <cmath>
#ifndef M_PI
#define M_PI    3.141592653589
#endif

using namespace std;

class Vector2D {
public :
    float x,y;
    Vector2D():x(0),y(0) {};
    Vector2D(const Vector2D &v) {x=v.x; y=v.y; }
    Vector2D(float p_x,float p_y):x(p_x),y(p_y) {};
    Vector2D operator-(const Vector2D &op2) const {
        return Vector2D(x - op2.x, y - op2.y);
    }
    Vector2D operator+(const Vector2D &op2) const {
        return Vector2D(x + op2.x, y + op2.y);
    }
    float operator*(const Vector2D &op2) const {
        return x*op2.x+y*op2.y;
    }
    void set(float p_x,float p_y) {
        x=p_x;
        y=p_y;
    }
    void normalize() {
        float norm=sqrt(x*x+y*y);
        x/=norm;
        y/=norm;
    }
    double norm() {
        return sqrt(x*x+y*y);
    }
    Vector2D ortho() {
        return Vector2D(-y,x);
    }
    friend ostream& operator<<(ostream& os,const Vector2D &v);
    friend istream& operator>>(istream& is,Vector2D &v);
};

Vector2D operator*(float a,const Vector2D &v);
ostream& operator<<(ostream& os,const Vector2D &v);
istream& operator>>(istream& is,Vector2D &v);
#endif //GEOMETRY_TRIANGULATION_GL_VECTOR2D_H