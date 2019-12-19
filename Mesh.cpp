//
// Created by bpiranda on 22/11/2019.
//
#include <glutWindow.h>
#include "Mesh.h"
#include "Polygon.h"
#include <list>

Mesh::Mesh(const float p_vert[][2], int p_nvert,const int p_tri[][3],int p_ntri) {
    for (int i=0; i<p_nvert; i++) {
        vertices.emplace_back(Vector2D(p_vert[i][0],p_vert[i][1]));
    }
    for (int i=0; i<p_ntri; i++) {
        triangles.emplace_back(Triangle(&vertices[p_tri[i][0]],
                &vertices[p_tri[i][1]],&vertices[p_tri[i][2]]));
    }
    checkDelaunay();
}

Mesh::Mesh(vector<Vector2D> &p_vert) {
    int n = p_vert.size();
    assert(n>=3);
    if (n==3) {
        for (auto v:p_vert) {
            vertices.emplace_back(Vector2D(v.x,v.y));
        }
        // order the 3 points
        Triangle T(&vertices[0],&vertices[1],&vertices[2]);
        if (T.isOnTheLeft(&vertices[2],&vertices[0],&vertices[1])) {
            triangles.push_back(T);
        } else {
            triangles.emplace_back(Triangle(&vertices[0],&vertices[2],&vertices[1]));
        }
    } else {
        MyPolygon CH(p_vert); // calculate convex hull
        Vector2D *ptrPt;
        int nCH = CH.getNbPts();
        for (int i=0; i<nCH; i++) {
            ptrPt=CH.getPtrPt(i);
            vertices.emplace_back(Vector2D(ptrPt->x,ptrPt->y));
        }

        // construct a triangulation of the hull
        for (int i=1; i<nCH-1; i++) {
            triangles.emplace_back(Triangle(&vertices[0],&vertices[i],&vertices[(i+1)%nCH]));
        }

        // add internal points
        auto v = p_vert.begin();
        while (v!=p_vert.end()) {
            if (!CH.isAVertex(*v)) {
                cout << "internal point:" << *v << endl;
                subdivide(*v);
            }
            v++;
        }
    }
    checkDelaunay();
}

void Mesh::draw() {
    for (auto t :triangles) {
        t.draw();
    }
    int i=0;
    glColor3fv(&BLACK[0]);
    for (auto p : vertices) {
        GlutWindow::drawText(p.x-10,p.y,to_string(i++),GlutWindow::ALIGN_RIGHT);
    }
    for (auto t :triangles) {
        t.drawCircle();
    }

}

void Mesh::onMouseMove(double x, double y) {
    Vector2D mousePos((float)x,(float)y);

    auto t = triangles.begin();
    while (t!=triangles.end()) {
        t->onMouseMove(mousePos);
        t++;
    }
}

bool Mesh::checkDelaunay() {
    bool test=true;
    auto t = triangles.begin();
    while (t!=triangles.end()) {
        test= t->checkDelaunay(vertices) && test;
        t++;
    }
    return test;
}

const Triangle* Mesh::getTriangle(const Vector2D &pt) const {
    auto t = triangles.begin();
    while (t!=triangles.end() && !t->isInside(pt)) {
        t++;
    }
    if (t==triangles.end()) {
        return nullptr;
    }
    return &(*t);
}

Triangle* Mesh::findTriangle(const Vector2D *P1,const Vector2D *P2) {
    auto t = triangles.begin();

    while (t!=triangles.end() && !((t->ptr[0]==P1 && t->ptr[1]==P2) ||
        (t->ptr[1]==P1 && t->ptr[2]==P2) || (t->ptr[2]==P1 && t->ptr[0]==P2))) {
        t++;
    }
    if (t==triangles.end()) {
        return nullptr;
    }
    return &(*t);
}

Triangle* Mesh::neighborInside(const Triangle *T) {
   Triangle *T1 = findTriangle(T->ptr[1],T->ptr[0]);
    if (T1) {
        if (T1->ptr[0]!=T->ptr[0] && T1->ptr[0]!=T->ptr[1] && T1->ptr[0]!=T->ptr[2] && T->isInsideCircle(*T1->ptr[0])) { return T1; }
        if (T1->ptr[1]!=T->ptr[0] && T1->ptr[1]!=T->ptr[1] && T1->ptr[1]!=T->ptr[2] && T->isInsideCircle(*T1->ptr[1])) { return T1; }
        if (T1->ptr[2]!=T->ptr[0] && T1->ptr[2]!=T->ptr[1] && T1->ptr[2]!=T->ptr[2] && T->isInsideCircle(*T1->ptr[2])) { return T1; }
    }

    T1 = findTriangle(T->ptr[2],T->ptr[1]);
    if (T1) {
        if (T1->ptr[0]!=T->ptr[0] && T1->ptr[0]!=T->ptr[1] && T1->ptr[0]!=T->ptr[2] && T->isInsideCircle(*T1->ptr[0])) { return T1; }
        if (T1->ptr[1]!=T->ptr[0] && T1->ptr[1]!=T->ptr[1] && T1->ptr[1]!=T->ptr[2] && T->isInsideCircle(*T1->ptr[1])) { return T1; }
        if (T1->ptr[2]!=T->ptr[0] && T1->ptr[2]!=T->ptr[1] && T1->ptr[2]!=T->ptr[2] && T->isInsideCircle(*T1->ptr[2])) { return T1; }
    }

    T1 = findTriangle(T->ptr[0],T->ptr[2]);
    if (T1) {
        if (T1->ptr[0]!=T->ptr[0] && T1->ptr[0]!=T->ptr[1] && T1->ptr[0]!=T->ptr[2] && T->isInsideCircle(*T1->ptr[0])) { return T1; }
        if (T1->ptr[1]!=T->ptr[0] && T1->ptr[1]!=T->ptr[1] && T1->ptr[1]!=T->ptr[2] && T->isInsideCircle(*T1->ptr[1])) { return T1; }
        if (T1->ptr[2]!=T->ptr[0] && T1->ptr[2]!=T->ptr[1] && T1->ptr[2]!=T->ptr[2] && T->isInsideCircle(*T1->ptr[2])) { return T1; }
    }

    return nullptr;
}

void Mesh::flip(Triangle &ptr1,Triangle &ptr2) {
    // search common edge
    Vector2D *P = ptr1.getVertexNotIn(ptr2);
    Vector2D *Q = ptr2.getVertexNotIn(ptr1);
    Vector2D *R = ptr1.getNextVertex(P);
    Vector2D *S = ptr2.getNextVertex(Q);
    ptr1.updateVertices(P,R,Q);
    ptr2.updateVertices(Q,S,P);
}

void Mesh::solveDelaunay() {
    // copy tabTriangles in a list
    list<Triangle*> processList;

    auto t = triangles.begin();
    while (t!=triangles.end()) {
        processList.push_back(&(*t));
        t++;
    }
    while (processList.size()>1) {
        Triangle *current = processList.front();
        processList.pop_front();
        cout << "Treat:" << current->whoami(vertices) << endl;
        if (!current->isDelaunay) {
            cout << "is not valid" << endl;
            Triangle *Tneighbor = neighborInside(current);

            if (Tneighbor!=nullptr) {
                cout << "Neighbor:" << Tneighbor->whoami(vertices) << endl;
                flip(*current,*Tneighbor);
                // remove Tneighbor form the list
                auto tr=processList.begin();
                while (tr!=processList.end() && (*tr)!=Tneighbor) {
                    tr++;
                }
                if (tr!=processList.end()) processList.erase(tr);
            } else {
                cout << "no neighbor" << endl;
                processList.push_back(current);
            }
        }
    }
}

void Mesh::subdivide(const Vector2D &P) {
    auto it=triangles.begin();
    while (it!=triangles.end() && !it->isInside(P)) it++;
    if (it!=triangles.end()) {
        int i = vertices.size();
        vertices.emplace_back(Vector2D(P.x,P.y));
        Vector2D *v[4] = {(*it).ptr[0],(*it).ptr[1],(*it).ptr[2],&vertices[i]};
        triangles.erase(it);
        triangles.emplace_back(Triangle(v[0], v[3], v[2]));
        triangles.emplace_back(Triangle(v[1], v[2], v[3]));
        triangles.emplace_back(Triangle(v[0], v[1], v[3]));
        checkDelaunay();
    }
}