//
// Created by bpiranda on 22/11/2019.
//

#ifndef GEOMETRY_TRIANGULATION_GL_TRIANGLE_H
#define GEOMETRY_TRIANGULATION_GL_TRIANGLE_H
#ifndef __APPLE__
#include <GL/freeglut.h>
#else
#include <GLUT/glut.h> // for APPLE only
#endif
#include <vector>
#include <string>
#include <array>
#include "Vector2D.h"
#include "determinant.h"

const array<float,4> BLACK={0.0,0.0,0.0,1.0f};
const array<float,4> GREY={0.75f,0.75f,0.75f,1.0f};
const array<float,4> RED={1.0f,0.0,0.0,1.0f};
const array<float,4> ORANGE={1.0f,0.27f,0.0,1.0f};
const array<float,4> YELLOW={1.0f,1.0f,0.0,1.0f};
const array<float,4> GREEN={0.0,1.0f,0.0,1.0f};
const array<float,4> BLUE={0.0,0.0,1.0f,1.0f};
const array<float,4> CYAN={0.0,1.0,1.0f,1.0f};
const array<float,4> PINK={1.0,0.75,0.80f,1.0f};
const float pointRadius=5.0;
const int pointSubs=8;

using namespace std;

class Triangle {
public:
    Vector2D *ptr[3];
    bool isDelaunay;
    bool isHighlighted;
    Vector2D circumCenter;
    float circumRadius;

    Triangle(Vector2D *ptr1,Vector2D *ptr2,Vector2D *ptr3) {
        ptr[0] = ptr1;
        ptr[1] = ptr2;
        ptr[2] = ptr3;
        isHighlighted = false;
        isDelaunay = false;
        calculateCircle();
    }

    void updateVertices(Vector2D *ptr1,Vector2D *ptr2,Vector2D *ptr3) {
        ptr[0] = ptr1;
        ptr[1] = ptr2;
        ptr[2] = ptr3;
        isHighlighted = false;
        isDelaunay = false;
        calculateCircle();
    }

    void calculateCircle() {
        Vector2D AC=*ptr[2]-*ptr[0];
        Vector2D AB=*ptr[1]-*ptr[0];
        Vector2D V=AC.ortho();

        float k=0.5f*(AB*AB-AC*AB)/(V*AB);

        circumCenter=*ptr[0]+(0.5*AC)+k*V;
        circumRadius = (float)((circumCenter-*ptr[0]).norm());
    }

    void onMouseMove(const Vector2D& pos) {
        isHighlighted=isInside(pos);
    }

    bool isOnTheLeft(const Vector2D *P,const Vector2D *P1,const Vector2D *P2) const {
        Vector2D AB = *P2-*P1,
                AP = *P-*P1;

        return (AB.x*AP.y - AB.y*AP.x)>=0;
    }

    bool isEmpty(const vector<Vector2D*> &tmp,int n) {
        auto p=tmp.begin()+n;
        while (p!=tmp.end() && (!isOnTheLeft(*p,ptr[0],ptr[1]) || !isOnTheLeft(*p,ptr[1],ptr[2]) || !isOnTheLeft(*p,ptr[2],ptr[0]))) {
            p++;
        }
        return p==tmp.end();
    }

    bool isInside(const Vector2D &P) const {
        return isOnTheLeft(&P,ptr[0],ptr[1]) && isOnTheLeft(&P,ptr[1],ptr[2]) && isOnTheLeft(&P,ptr[2],ptr[0]);
    }

    void draw() {
        glColor4fv(isDelaunay?(isHighlighted?&GREEN[0]:&GREY[0]):(isHighlighted?&ORANGE[0]:&YELLOW[0]));
        glBegin(GL_TRIANGLES);
        glVertex2f(ptr[0]->x,ptr[0]->y);
        glVertex2f(ptr[1]->x,ptr[1]->y);
        glVertex2f(ptr[2]->x,ptr[2]->y);
        glEnd();
        glColor3fv(&BLACK[0]);
        glBegin(GL_LINE_LOOP);
        glVertex2f(ptr[0]->x,ptr[0]->y);
        glVertex2f(ptr[1]->x,ptr[1]->y);
        glVertex2f(ptr[2]->x,ptr[2]->y);
        glEnd();
    }

    void drawCircle() {
        // draw circle
        if (isHighlighted) {
            glColor3fv(&RED[0]);
            glPushMatrix();
            glTranslatef(circumCenter.x, circumCenter.y, 0);
            glLineWidth(3);
            glBegin(GL_LINE_LOOP);
            float a = 0.0;
            for (int i = 0; i < 40; i++) {
                glVertex2f(circumRadius * cos(a), circumRadius * sin(a));
                a += (float) (M_PI / 20.0);
            }
            glEnd();
            glLineWidth(1);
            glPopMatrix();
        } else {
            glColor3fv(&RED[0]);
            glPushMatrix();
            glTranslatef(circumCenter.x, circumCenter.y, 0);
            glBegin(GL_POLYGON);
            float a = 0.0;
            for (int i = 0; i < pointSubs; i++) {
                glVertex2f(pointRadius * cos(a), pointRadius * sin(a));
                a += (float) (2.0*M_PI / pointSubs);
            }
            glEnd();
            glPopMatrix();
        }
    }

    bool isInsideCircle(const Vector2D &P) const {
        Matrix33 mat33;

        mat33.m[0][0] = ptr[0]->x-P.x; mat33.m[0][1] = ptr[0]->y-P.y;
        mat33.m[0][2] = ptr[0]->x*ptr[0]->x-P.x*P.x + ptr[0]->y*ptr[0]->y-P.y*P.y;
        mat33.m[1][0] = ptr[1]->x-P.x; mat33.m[1][1] = ptr[1]->y-P.y;
        mat33.m[1][2] = ptr[1]->x*ptr[1]->x-P.x*P.x + ptr[1]->y*ptr[1]->y-P.y*P.y;
        mat33.m[2][0] = ptr[2]->x-P.x; mat33.m[2][1] = ptr[2]->y-P.y;
        mat33.m[2][2] = ptr[2]->x*ptr[2]->x-P.x*P.x + ptr[2]->y*ptr[2]->y-P.y*P.y;
        return mat33.determinant()>0;
    }

    bool checkDelaunay(const vector<Vector2D> &points) {
        auto p=points.begin();
        while (p!=points.end() && !isInsideCircle(*p)) {
            p++;
        }
        isDelaunay=(p==points.end());
        return isDelaunay;
    }

    Vector2D *getVertexNotIn(const Triangle &T) const {
        if (ptr[0]!=T.ptr[0] && ptr[0]!=T.ptr[1] && ptr[0]!=T.ptr[2]) return ptr[0];
        if (ptr[1]!=T.ptr[0] && ptr[1]!=T.ptr[1] && ptr[1]!=T.ptr[2]) return ptr[1];
        if (ptr[2]!=T.ptr[0] && ptr[2]!=T.ptr[1] && ptr[2]!=T.ptr[2]) return ptr[2];
        return nullptr;
    }

    Vector2D *getNextVertex(const Vector2D *pt) const {
        if (pt==ptr[0]) return ptr[1];
        if (pt==ptr[1]) return ptr[2];
        if (pt==ptr[2]) return ptr[0];
        return nullptr;
    }

    Vector2D *getPrevVertex(const Vector2D *pt) const {
        if (pt==ptr[0]) return ptr[2];
        if (pt==ptr[1]) return ptr[0];
        if (pt==ptr[2]) return ptr[1];
        return nullptr;
    }

    bool hasVertex(const Vector2D *pt) const {
        return pt==ptr[0] || pt==ptr[1] || pt==ptr[2];
    }

    Vector2D nextEdgeNormal(const Vector2D *pt) const {
        if (pt==ptr[0]) return Vector2D(ptr[1]->y-ptr[0]->y,-(ptr[1]->x-ptr[0]->x));
        if (pt==ptr[1]) return Vector2D(ptr[2]->y-ptr[1]->y,-(ptr[2]->x-ptr[1]->x));
        if (pt==ptr[2]) return Vector2D(ptr[0]->y-ptr[2]->y,-(ptr[0]->x-ptr[2]->x));
        return Vector2D(0,0);
    }

    Vector2D previousEdgeNormal(const Vector2D *pt) const {
        if (pt==ptr[0]) return Vector2D(-(ptr[2]->y-ptr[0]->y),ptr[2]->x-ptr[0]->x);
        if (pt==ptr[1]) return Vector2D(-(ptr[0]->y-ptr[1]->y),ptr[0]->x-ptr[1]->x);
        if (pt==ptr[2]) return Vector2D(-(ptr[1]->y-ptr[2]->y),ptr[1]->x-ptr[2]->x);
        return Vector2D(0,0);
    }

    string whoami(const vector<Vector2D>& points) const {
        return string("Triangle (") + to_string((int)(ptr[0]-&points[0])) +
        string(",") + to_string((int)(ptr[1]-&points[0])) + string(",") +
        to_string((int)(ptr[2]-&points[0])) + string(")");

    }

    bool operator!= (const Triangle &t) {
        return (ptr[0]!=t.ptr[0] || ptr[1]!=t.ptr[1] || ptr[2]!=t.ptr[2]);
    }

    static array<float,4> getColorByName(const string &name) {
        if (name.substr(0,3)=="RED") return RED;
        else if (name.substr(0,4)=="BLUE") return BLUE;
        else if (name.substr(0,5)=="GREEN") return GREEN;
        else if (name.substr(0,4)=="CYAN") return CYAN;
        else if (name.substr(0,6)=="YELLOW") return YELLOW;
        else if (name.substr(0,4)=="PINK") return PINK;
        return BLACK;
    }

    double surface() const {
        double a=(*ptr[1]-*ptr[0]).norm();
        double b=(*ptr[2]-*ptr[1]).norm();
        double c=(*ptr[0]-*ptr[2]).norm();
        double p=0.5*(a+b+c);

        return sqrt(p*(p-a)*(p-b)*(p-c));
    }
};

#endif //GEOMETRY_TRIANGULATION_GL_TRIANGLE_H
