#include <iostream>
#include <cstdlib>
#include <vector>
#include <glutWindow.h>
#include <fstream>
#include "Mesh.h"
#include "Voronoi.h"
#include "Icon.h"

using namespace std;

class ServerData {
public:
    string name;
    Vector2D position;
    string colorName;

    ServerData() {};
    ServerData(const string &p_name,const Vector2D &p_position,const string &p_colorName):
    name(p_name),position(p_position),colorName(p_colorName)  {};
};

class PolygonDraw: public GlutWindow {
private:
    Mesh *mesh;
    Voronoi *voronoi;
    bool showVoronoi;
    bool vertexMotionMode;
    Icon *server;
    vector<Vector2D>::iterator mouseOverServer;
    vector<ServerData> servers;
public:

    PolygonDraw(const string &title,int argc,char **argv):GlutWindow(argc,argv,title,1000,800,FIXED) {
        voronoi= nullptr;
        mesh= nullptr;
        showVoronoi=false;
        vertexMotionMode=false;
        server = new Icon(64.0,64.0,"../Textures/server.tga");
    };

    void onStart() override;
    void onDraw() override;
    void onQuit() override;
    void onMouseMove(double cx,double cy) override;
    void onMouseDrag(double cx,double cy) override;
    void onMouseDown(int button,double cx,double cy) override;
    void onKeyPressed(unsigned char c,double x,double y) override;
    void loadServerPositions(const string &title);
    bool saveServerPositions(const string &title);
};

void PolygonDraw::onStart() {
    float tabVect[][2]={{280,740},{700,750},{500,700}};
    int tabTri[][3]={{0,2,1}};
    mesh = new Mesh(tabVect,3,tabTri,1);
    mouseOverServer = mesh->vertices.end();
    glClearColor(1.0,1.0,1.0,1.0); // background color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void PolygonDraw::onDraw() {
    // draw the referential
    glPushMatrix();
    glTranslatef(10,10,0);
    glColor3fv(&RED[0]);
    glBegin(GL_QUADS);
    glVertex2f(0.0,-2.0);
    glVertex2f(100.0,-2.0);
    glVertex2f(100.0,2.0);
    glVertex2f(0.0,2.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(110.0,0.0);
    glVertex2f(90.0,-10.0);
    glVertex2f(90.0,10.0);
    glEnd();
    glColor3fv(&GREEN[0]);
    glBegin(GL_QUADS);
    glVertex2f(-2.0,0.0);
    glVertex2f(2.0,0.0);
    glVertex2f(2.0,100.0);
    glVertex2f(-2.0,100.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0,110.0);
    glVertex2f(-10.0,90.0);
    glVertex2f(10.0,90.0);
    glEnd();
    glPopMatrix();

    mesh->draw();

    for (auto v:mesh->vertices) {
        glPushMatrix();
        glTranslatef(v.x,v.y,0.0);
        server->glDraw();
        glPopMatrix();
    }
    if (voronoi && showVoronoi) {
        voronoi->draw();
    }
}
void PolygonDraw::onQuit() {
    delete mesh;
    delete voronoi;
    delete server;
}

void PolygonDraw::onMouseMove(double cx, double cy) {
    static float w = server->getWidth()/2;
    static float h = server->getHeight()/2;
    //mesh->onMouseMove(cx,cy);
    auto m_it=mesh->vertices.begin();
    while (m_it!=mesh->vertices.end() && (cx>m_it->x+w || cx<m_it->x-w || cy>m_it->y+h || cy<m_it->y-w)) {
        m_it++;
    }
    mouseOverServer =m_it;
    if (mouseOverServer!=mesh->vertices.end()) {
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    } else {
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
}

void PolygonDraw::onMouseDrag(double cx, double cy) {
    if (mouseOverServer!=mesh->vertices.end()) {
        mouseOverServer->x=cx;
        mouseOverServer->y=cy;
    }
}

void PolygonDraw::onMouseDown(int button, double cx, double cy) {
    /*auto itPoint = mesh->getVertex();
    if (itPoint!=mesh->vertices.end()) {
        vertexMotionMode=true;
    }
    Vector2D P((float)cx,(float)cy);
    const Triangle *T = mesh->getTriangle(P);

    if (T) {
        cout << "Clicked:" << T->whoami(mesh->vertices) << endl;
        if (button==GLUT_RIGHT_BUTTON) {
            mesh->subdivide(P);
        } else {
            if (!T->isDelaunay) {
                // search a Triangle which shares an edge and has a vertex inside T circumcircle
                Triangle *Tneighbor = mesh->neighborInside(T);

                if (Tneighbor) {
                    cout << "Neighbor:" << Tneighbor->whoami(mesh->vertices) << endl;
                    // flip the vertices of T and Tneighbor
                    mesh->flip(*T, *Tneighbor);
                }
            }
        }
        if (mesh->checkDelaunay() && voronoi == nullptr) {
            voronoi = new Voronoi(*mesh);
        }
    }*/
}

void PolygonDraw::onKeyPressed(unsigned char c, double x, double y) {
    if (c=='c') {
        mesh->solveDelaunay();
        mesh->checkDelaunay();
        voronoi = new Voronoi(*mesh);
    } else if (c=='v') showVoronoi = !showVoronoi;
}

void PolygonDraw::loadServerPositions(const string &title) {
    ifstream fin(title);
    string name,color;
    Vector2D position;

    servers.clear();
    if (fin.is_open()) {
        while (!fin.eof()) {
            fin >> name >> position >> color;
            servers.push_back(ServerData(name,position,color));
        }
    }
    fin.close();
}

bool PolygonDraw::saveServerPositions(const string &title) {
    ofstream fout(title);

    if (fout.is_open()) {
        for (auto it:servers) {
            fout << it.name << ";" << it.position << ":" << it.colorName << "\n";
        }
    } else {
        return false;
    }
    return true;
}

int main(int argc,char **argv) {
    PolygonDraw win("Triangulation", argc,argv);
    win.start();

    return 0;
}





