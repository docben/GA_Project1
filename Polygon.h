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

    inline int getNbPts() { return N; };
    inline Vector2D* getPtrPt(int i) { return &tabPts[i]; };

    void triangulation();

    bool addPoint(const Vector2D &p) {
        if (Nmax==N-2) return false;
        tabPts[N++]=p;
        tabPts[N]=tabPts[0];
        return true;
    }

    bool insertPoint(const Vector2D &p,int index) {
        if (Nmax==N-2) return false;
        for (int i=N; i>index; i--) {
            tabPts[i]=tabPts[i-1];
        }
        tabPts[index]=p;
        tabPts[++N]=tabPts[0];
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

    /**
     * @brief check is a point (x,y) is inside the triangle of the polygon
     * @param x: component of the point
     * @param y: component of the point
     * @return true if inside
     */
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

    void clip(int x0,int y0,int x1,int y1);
    void print() {
        for (int i=0; i<N;i++) {
            cout << i << ":" << tabPts[i] << endl;
        }
    }

    double surface() {
        if (tabTriangles.size()==0) triangulation();
        double s=0;
        for (auto t:tabTriangles) {
            s+=t.surface();
        }
        return s;
    }

    bool isAVertex(const Vector2D &pt) {
        int i=0;
        Vector2D *ptr = tabPts;
        while (i<N && (pt.x!=ptr->x || pt.y!=ptr->y)) {
            i++;
            ptr++;
        }
        return (i!=N);
    }
};

#endif //GEOMETRY_TRIANGULATION_GL_POLYGON_H
