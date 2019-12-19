//
// Created by bpiranda on 14/12/2019.
//
#include "Vector2D.h"

Vector2D operator*(float a,const Vector2D &v) {
    return Vector2D(a*v.x,a*v.y);
}

ostream& operator<<(ostream& os,const Vector2D &V) {
    os << "(" << V.x << "," << V.y << ")";
    return os;
}

istream& operator>>(istream& is,Vector2D &V) {
    is >> V.x;
    is >> V.y;

    return is;
}
