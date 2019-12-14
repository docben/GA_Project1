//
// Created by bpiranda on 10/12/2019.
//

//
// Created by bpiranda on 19/11/2019.
//

#include <cmath>
#include <glutWindow.h>
#include "Polygon.h"
#include <stack>
#include <algorithm>

using namespace std;

bool polarComparison(Vector2D P1,Vector2D P2) {
    double a1 = asin(P1.y/sqrt(P1.x*P1.x+P1.y*P1.y));
    if (P1.x<0.0) a1=M_PI-a1;
    double a2 = asin(P2.y/sqrt(P2.x*P2.x+P2.y*P2.y));
    if (P2.x<0.0) a2=M_PI-a2;
    return a1<a2;
}

MyPolygon::MyPolygon(vector<Vector2D> &points) {
    assert(points.size()>3);
    auto p=points.begin();
    auto pymin=points.begin();

    // find point with minimal y and swap with first point
    while (p!=points.end()) {
        if (p->y<pymin->y) {
            pymin=p;
        }
        p++;
    }
    // swap
    if (pymin!=points.begin()) iter_swap(points.begin(), pymin);

    Vector2D origin(points.begin()->x,points.begin()->y);
    // copy points in a set of points relative to points[0]
    vector<Vector2D> pointsRelative;
    for (auto pOrig:points) {
        pointsRelative.push_back(Vector2D(pOrig.x-origin.x,pOrig.y-origin.y));
    }

    // sorting point with angular criteria
    sort(pointsRelative.begin()+1,
         pointsRelative.end(),polarComparison);

    stack<Vector2D*> CHstack;
    Vector2D *top_1,*top;
    CHstack.push(&pointsRelative[0]);
    CHstack.push(&pointsRelative[1]);
    CHstack.push(&pointsRelative[2]);

    vector<Vector2D>::iterator pi=pointsRelative.begin()+3;
    while (pi!=pointsRelative.end()) {
        top = CHstack.top();
        CHstack.pop();
        top_1 = CHstack.top();
        CHstack.push(top);
        while (!isOnTheLeft(&(*pi),top_1,top)) {
            CHstack.pop();
            // update values of top and top_1
            top = CHstack.top();
            CHstack.pop();
            top_1 = CHstack.top();
            CHstack.push(top);
        }
        CHstack.push(&(*pi));
        pi++;
    }

    // get stack points to create current polygon
    N=CHstack.size();
    Nmax = N;
    tabPts = new Vector2D[Nmax+1];
    int i=N-1;
    while (!CHstack.empty()) {
        tabPts[i--]=*(CHstack.top())+origin;
        CHstack.pop();
    }
    tabPts[N]=tabPts[0];
}

void MyPolygon::triangulation() {
    vector<Vector2D*> tmp;
    // copy the list of vertices into the tmp list
    for (int i=0; i<N; i++) {
        tmp.push_back(&(tabPts[i]));
    }
    int n=N;
    // while we can add a triangle to tabTriangles
    while (n>2) {
        int i=0;
        auto p = tmp.begin();
        bool test;
        // create a triangle using p,p+1 and p+2 as vertices
        Triangle T(*p,*(p+1),*(p+2));
        do { // search a triangle without another points inside
            test=!T.isEmpty(tmp,i+3);
            if (test) {
                i++;
                p++;
                T=Triangle(*p,*(p+1),*(p+2));
            }
        } while (i<n-2 && test);
        assert(i<n-2);
        // add T to tabTriangles
        tabTriangles.push_back(T);
        tmp.erase(p+1); // remove point(p+1) from tmp;
        n--; // or n=tmp.size();
    }
}

void MyPolygon::draw() {
    if (tabTriangles.empty()) {
        triangulation();
    }

    // draw the interior
    glColor4fv(&color[0]);
    /*glBegin(GL_POLYGON);
    for (int i=0; i<N; i++) {
        glVertex2f(tabPts[i].x,tabPts[i].y);
    }
    glEnd();
    glBegin(GL_TRIANGLES);
    for (int i=0; i<N-1; i++) {
        glVertex2f(tabPts[0].x,tabPts[0].y);
        glVertex2f(tabPts[i].x,tabPts[i].y);
        glVertex2f(tabPts[i+1].x,tabPts[i+1].y);
    }
    glEnd();*/

    glBegin(GL_TRIANGLES);
    for (auto t:tabTriangles) {
        glVertex2f(t.ptr[0]->x,t.ptr[0]->y);
        glVertex2f(t.ptr[1]->x,t.ptr[1]->y);
        glVertex2f(t.ptr[2]->x,t.ptr[2]->y);
    }
    glEnd();
    // draw the border
    /*glColor3fv(BLUE);
    glBegin(GL_LINE_LOOP);
    for (auto t:tabTriangles) {
        glVertex2f(t.ptr[0]->x,t.ptr[0]->y);
        glVertex2f(t.ptr[1]->x,t.ptr[1]->y);
        glVertex2f(t.ptr[2]->x,t.ptr[2]->y);
    }
    glEnd();*/

    glColor3fv(&BLACK[0]);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<N; i++) {
        glVertex2f(tabPts[i].x,tabPts[i].y);
    }
    glEnd();
    glLineWidth(1);
    // draw the number of points
    /*for (int i=0; i<N; i++) {
        glBegin(GL_LINES);
        glVertex2f(tabPts[i].x-10,tabPts[i].y-10);
        glVertex2f(tabPts[i].x+10,tabPts[i].y+10);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(tabPts[i].x+10,tabPts[i].y-10);
        glVertex2f(tabPts[i].x-10,tabPts[i].y+10);
        glEnd();
        GlutWindow::drawText(tabPts[i].x-10,tabPts[i].y,to_string(i),GlutWindow::ALIGN_RIGHT);
    }*/
}
