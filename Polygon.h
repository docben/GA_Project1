//
// Created by bpiranda on 10/12/2019.
//

#ifndef GEOMETRY_TRIANGULATION_GL_POLYGON_H
#define GEOMETRY_TRIANGULATION_GL_POLYGON_H
#include "Vector2D.h"
#include "Triangle.h"
#include <vector>
#include <array>

using namespace std;
class MyPolygon {
    int Nmax;
    Vector2D *tabPts;
    int N;
    vector <Triangle> tabTriangles;
    array<float,4> color;
public:
    MyPolygon(int p_Nmax): Nmax(p_Nmax) {
        N=0;
        tabPts = new Vector2D[Nmax];
        setColor(YELLOW);
    }

    MyPolygon(vector<Vector2D> &points);

    ~MyPolygon() {
        delete [] tabPts;
    }

    void triangulation();

    bool addPoint(const Vector2D &p) {
        if (Nmax==N-2) return false;
        tabPts[N++]=p;
        tabPts[N]=tabPts[0];
        return true;
    }

    void draw();

    bool isOnTheLeft(const Vector2D &P,int i) {
        Vector2D AB = tabPts[i+1]-tabPts[i],
                AP = P-tabPts[i];
        return (AB.x*AP.y - AB.y*AP.x)>=0;
    }

    bool isOnTheLeft(const Vector2D *P,const Vector2D *P1,const Vector2D *P2) {
        Vector2D AB = *P2-*P1,
                AP = *P-*P1;
        return (AB.x*AP.y - AB.y*AP.x)>=0; // z component of AB X AP
    }

    bool isConvex() {
        int i=0;
        while (i<N && isOnTheLeft(tabPts[(i+2)%N],i)) {
            i++;
        }
        return (i==N);
    }

    bool isInside(float x,float y) {
        Vector2D P(x,y);
        int i=0;
        while (i<N && isOnTheLeft(P,i)) {
            i++;
        }
        return (i==N);
    }

    bool isInsideTriangles(float x,float y) {
        Vector2D P(x,y);
        auto triangle=tabTriangles.begin();
        while (triangle!=tabTriangles.end() && !(*triangle).isInside(P)) {
            triangle++;
        }
        return (triangle!=tabTriangles.end());
    }

    void setColor(const array<float,4> &t_color) {
        color = t_color;

    }

};

#endif //GEOMETRY_TRIANGULATION_GL_POLYGON_H
