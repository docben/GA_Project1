//
// Created by bpiranda on 10/12/2019.
//

#include "Voronoi.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <glutWindow.h>

const array<float,4> tabColors[6]={{1.0f,0.0,0.0,0.5f},{1.0f,0.27f,0.0,0.5f},{1.0f,1.0f,0.0,0.5f},{0.0f,1.0f,0.0,0.5f},
{0.0f,1.0f,1.0,0.5f},{0.0f,0.0f,1.0,0.5f}};

Voronoi::Voronoi(const Mesh &mesh) {
   auto m_vert = mesh.vertices.begin();
   int currentColor=0;
   vector<const Triangle*> tabTri;
   while (m_vert!=mesh.vertices.end()) {
        auto mt_it = mesh.triangles.begin();
        tabTri.clear();
        while (mt_it!=mesh.triangles.end()) {
            if ((*mt_it).hasVertex(&(*m_vert))) {
                tabTri.push_back(&(*mt_it));
                cout << "Add triangle:" << (*mt_it).whoami(mesh.vertices) << endl;
            }
            mt_it++;
        }
        // find left border
        auto first = tabTri.begin();
        auto tt_it = tabTri.begin();
        bool found=false;
        while (tt_it!=tabTri.end() && !found) {
            auto comp_it = tabTri.begin();
            while (comp_it!=tabTri.end() && (*tt_it)->getNextVertex(&(*m_vert))!=(*comp_it)->getPrevVertex(&(*m_vert))) {
                comp_it++;
            }
            if (comp_it==tabTri.end()) {
                first=tt_it;
                found=true;
            }
            tt_it++;
        }
        if (found) {
            cout << "border found :" << (*first)->whoami(mesh.vertices) << endl;
        } else {
            cout << "no border" << endl;
        }
        // create polygon
        auto poly = new MyPolygon(20);
        polygons.push_back(poly);

        poly->setColor(tabColors[currentColor]);
        currentColor = (currentColor+1)%6;
        tt_it=first;
        if (found && GlutWindow::isInRect((*tt_it)->circumCenter.x,(*tt_it)->circumCenter.y,0,0,GlutWindow::getWindowWidth(),GlutWindow::getWindowHeight())) { // add a point for the left border
            Vector2D V = (*first)->nextEdgeNormal(&(*m_vert));
            cout << V << endl;
            float k;
            if (V.x > 0) { // (circumCenter+k V).x=width
                k = (GlutWindow::getWindowWidth() - (*first)->circumCenter.x) / V.x;
            } else {
                k = (-(*first)->circumCenter.x) / V.x;
            }
            if (V.y > 0) { // (circumCenter+k V).x=width
                k = std::min(k, (GlutWindow::getWindowHeight() - (*first)->circumCenter.y) / V.y);
            } else {
                k = std::min(k, (-(*first)->circumCenter.y) / V.y);
            }
            poly->addPoint(Vector2D((*first)->circumCenter + k * V));
            Vector2D pt = (*first)->circumCenter + k * V;
            cout << "newPoint=" << pt << endl;
        }
        auto comp_it = first;
        do {
            poly->addPoint(Vector2D((*tt_it)->circumCenter));
            cout << "newPoint=" << (*tt_it)->circumCenter << endl;
            // search triangle on right of tt_it
            comp_it = tabTri.begin();
            while (comp_it!=tabTri.end() && (*tt_it)->getPrevVertex(&(*m_vert))!=(*comp_it)->getNextVertex(&(*m_vert))) {
                comp_it++;
            }
            if (comp_it!=tabTri.end()) tt_it = comp_it;
        } while (tt_it!=first && comp_it!=tabTri.end());
       if (found && GlutWindow::isInRect((*tt_it)->circumCenter.x,(*tt_it)->circumCenter.y,0,0,GlutWindow::getWindowWidth(),GlutWindow::getWindowHeight())) { // add a point for the right border
           Vector2D V = (*tt_it)->previousEdgeNormal(&(*m_vert));
           float k;
           if (V.x > 0) { // (circumCenter+k V).x=width
               k = (GlutWindow::getWindowWidth() - (*tt_it)->circumCenter.x) / V.x;
           } else {
               k = (-(*tt_it)->circumCenter.x) / V.x;
           }
           if (V.y > 0) { // (circumCenter+k V).y=height
               k = std::min(k, (GlutWindow::getWindowHeight() - (*tt_it)->circumCenter.y) / V.y);
           } else {
               k = std::min(k, (-(*tt_it)->circumCenter.y) / V.y);
           }
           poly->addPoint(Vector2D((*tt_it)->circumCenter + k * V));
           Vector2D pt = (*tt_it)->circumCenter + k * V;
           cout << "newPoint=" << pt << endl;
       }
        //poly->clip(0,0,GlutWindow::getWindowWidth(),GlutWindow::getWindowHeight())
        m_vert++;
   }
}

Voronoi::~Voronoi() {
    // free polygons
}

void Voronoi::draw() {
    glBegin(GL_LINES);
    for (auto p:polygons) {
        p->draw();
    }
    glEnd();
}