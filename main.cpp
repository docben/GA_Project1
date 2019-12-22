#include <iostream>
#include <cstdlib>
#include <array>
#include <vector>
#include <glutWindow.h>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "Mesh.h"
#include "Voronoi.h"
#include "Icon.h"
#include "Drone.h"

using namespace std;

const string serversFileName("../Data/servers.txt");
const array<Vector2D,4> tabAppearingPos={ Vector2D(50.0f,50.0f),
                                          Vector2D(950.0f,50.0f),
                                          Vector2D(950.0f,750.0f),
                                          Vector2D(50.0f,750.0f)};

class PolygonDraw: public GlutWindow {
private:
    Mesh *mesh;
    Voronoi *voronoi;
    bool showVoronoi;
    Icon *server;
    Icon *drone;
    vector<DroneData*> drones;
    vector<ServerData*> servers;
    vector<ServerData*>::iterator mouseOverServer;
    ServerData* selectedServer;
    string configFile;
    double fieldSurface;
public:

    PolygonDraw(const string &title,int argc,char **argv):GlutWindow(argc,argv,title,1000,800,FIXED) {
        showVoronoi=false;
        mesh= nullptr;
        voronoi= nullptr;
        configFile = serversFileName;
        selectedServer=nullptr;
        fieldSurface = width*height;
        cout << "argc=" << argc << endl;
        int i=0;
        while (i<argc) {
            cout << "argv[" << i << "]=" << argv[i] << endl;
            if (strncmp(argv[i],"-c",2)==0) {
                i++;
                if (i<argc) {
                    configFile=argv[i++];
                }
            }
            i++;
        }
    };

    void onStart() override;
    void onDraw() override;
    void onQuit() override;
    void onMouseMove(double cx,double cy) override;
    void onMouseDrag(double cx,double cy) override;
    void onMouseDown(int button,double cx,double cy) override;
    void onKeyPressed(unsigned char c,double x,double y) override;
    void onUpdate(double dt) override;
    void loadServerPositions(const string &title);
    bool saveServerPositions(const string &title);
    void addDrone(const Vector2D &pos);
};

void PolygonDraw::onStart() {
    server = new Icon(48.0,48.0,"../Textures/server.tga");
    drone = new Icon(64,64,"../Textures/red_drone.tga");
    loadServerPositions(configFile);

    vector<Vector2D> tabVertices;
    for(auto s:servers) {
        tabVertices.push_back(s->position);
    }
    mesh = new Mesh(tabVertices);

    mouseOverServer = servers.end();
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

    if (voronoi && showVoronoi) {
        voronoi->draw();
    } else if (mesh) {
        mesh->draw();
    }

    for (auto d:drones) {
        glPushMatrix();
        glTranslatef(d->position.x,d->position.y,0.0);
        drone->glDraw();
        d->glDraw();
        glPopMatrix();
    }

    glColor3fv(&BLACK[0]);
    for (auto v:servers) {
        glPushMatrix();
        glTranslatef(v->position.x,v->position.y,0.0);
        server->glDraw();
        drawText(0,-0.5*server->getHeight()-18,v->name,ALIGN_CENTER);
        string s = to_string(v->links2Drone.size())+"/"+to_string((int)(v->surfaceRate*drones.size()+0.5));
        drawText(0,-0.5*server->getHeight()-36,s,ALIGN_CENTER);
        glPopMatrix();
    }

}
void PolygonDraw::onQuit() {
    delete mesh;
    delete voronoi;
    delete server;
    delete drone;
}

void PolygonDraw::onMouseMove(double cx, double cy) {
    static float w = server->getWidth()/2;
    static float h = server->getHeight()/2;
    auto m_it=servers.begin();
    while (m_it!=servers.end() && (cx>(*m_it)->position.x+w || cx<(*m_it)->position.x-w ||
        cy>(*m_it)->position.y+h || cy<(*m_it)->position.y-w)) {
        m_it++;
    }
    mouseOverServer =m_it;
    if (mouseOverServer!=servers.end()) {
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    } else {
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
    mesh->onMouseMove(cx,cy);
}

void PolygonDraw::onMouseDrag(double cx, double cy) {
    if (mouseOverServer!=servers.end()) {
        (*mouseOverServer)->position.x=(float)cx;
        (*mouseOverServer)->position.y=(float)cy;
    }
}

void PolygonDraw::onMouseDown(int button, double cx, double cy) {

}

void PolygonDraw::onKeyPressed(unsigned char c, double x, double y) {
    switch (c) {
        case 's':
            saveServerPositions(configFile);
        break;
        case 'v': showVoronoi=!showVoronoi; break;
        case 'z': {
            mesh->checkDelaunay();
            mesh->solveDelaunay();
            mesh->checkDelaunay();
            voronoi = new Voronoi(*mesh);
            voronoi->triangulatePolygons();
            auto s = servers.begin();
            while (s != servers.end()) {
                (*s)->ptrPolygon = voronoi->findPolygon((*s)->position);
                assert((*s)->ptrPolygon!= nullptr);
                array<float, 4> c = Triangle::getColorByName((*s)->colorName);

                (*s)->ptrPolygon->setColor(c);
                (*s)->surfaceRate = (*s)->ptrPolygon->surface()/fieldSurface;
                cout << (*s)->name << ":" << 0.1 * floor(((*s)->surfaceRate) * 1000 + 0.5) << endl;
                // construct neighbor list -------------------------------
                auto m_vert = mesh->vertices.begin(); // point of the mesh corresponding to s->position
                while (m_vert!=mesh->vertices.end() && (*m_vert!=(*s)->position)) {
                    m_vert++;
                }
                assert(m_vert!=mesh->vertices.end());

                auto mt_it = mesh->triangles.begin();
                vector<const Vector2D*> tabPts; // tabPts: list of vertices connected to m_vert

                while (mt_it!=mesh->triangles.end()) {
                    if ((*mt_it).hasVertex(&(*m_vert))) {
                        tabPts.push_back((*mt_it).getPrevVertex(&(*m_vert)));
                        tabPts.push_back((*mt_it).getNextVertex(&(*m_vert)));
                    }
                    mt_it++;
                }
                sort(tabPts.begin(),tabPts.end(),[](const Vector2D *a, const Vector2D *b) {
                    return (a->x > b->x)||((a->x==b->x)&&(a->y > b->y));
                });
                // add unique neighbor
                cout << "Neighbors: ";
                Vector2D prev;
                for (auto pp:tabPts) {
                    if (*pp!=prev) {
                        auto it_s = servers.begin();
                        while (it_s != servers.end() && ((*it_s)->position != *pp)) {
                            it_s++;
                        }
                        assert(it_s != servers.end());
                        (*s)->addNeighbor(*it_s);
                        cout << (*it_s)->name << ", ";
                    }
                    prev=*pp;
                }
                cout << endl;
                // end construct neighbor list ---------------------------


                s++;
            }
            showVoronoi=true;
        } break;
        case 'D' :
            addDrone(tabAppearingPos[rand()%tabAppearingPos.size()]);
        break;
        case 'd':
            addDrone(Vector2D(50,50));
        break;
    }
}

void PolygonDraw::onUpdate(double dt) {
    // initialize every drone forces
    auto d=drones.begin();
    while (d!=drones.end()) {
        (*d)->updateSpeed(dt);
        (*d)->sumF.init();
        d++;
    }
    // Associate drones
    d=drones.begin();
    while (d!=drones.end()) {
        if ((*d)->ptrServer==nullptr || !(*d)->ptrServer->droneIsOver((*d)->position)) {
            if ((*d)->ptrServer!= nullptr) {
                (*d)->ptrServer->removeDrone(*d);
            }
            auto s=servers.begin();
            while (s!=servers.end() && !(*s)->droneIsOver((*d)->position)){
                s++;
            }
            assert(s!=servers.end());
            cout << (*s)->name << endl;
            (*s)->addDrone(*d);
            (*d)->ptrServer=*s;
        }
        // collisions of d with others
        auto dcomp=d+1;
        while (dcomp!=drones.end()) {
            Vector2D AB = (*dcomp)->position-(*d)->position;
            double dAB=AB.norm();
            if (dAB<dmax) {
                float F=(float)((dAB>dmin)?(Fmax/dAB)*(dAB-dmax)/(dmin-dmax):Fmax);
                (*d)->sumF-=F*AB;
                (*dcomp)->sumF+=F*AB;
            }
            dcomp++;
        }
        // collision of d with borders
        double dAB=(*d)->position.x;
        if (dAB<dmax) {
            float F=(float)((dAB>dmin)?(Fmax/dAB)*(dAB-dmax)/(dmin-dmax):Fmax);
            (*d)->sumF+=Vector2D(F,0);
        }
        dAB=width-(*d)->position.x;
        if (dAB<dmax) {
            float F=(float)((dAB>dmin)?(Fmax/dAB)*(dAB-dmax)/(dmin-dmax):Fmax);
            (*d)->sumF+=Vector2D(-F,0);
        }
        dAB=(*d)->position.y;
        if (dAB<dmax) {
            float F=(float)((dAB>dmin)?(Fmax/dAB)*(dAB-dmax)/(dmin-dmax):Fmax);
            (*d)->sumF+=Vector2D(0,F);
        }
        dAB=height-(*d)->position.y;
        if (dAB<dmax) {
            float F=(float)((dAB>dmin)?(Fmax/dAB)*(dAB-dmax)/(dmin-dmax):Fmax);
            (*d)->sumF+=Vector2D(0,-F);
        }
        d++;
    }
}


void PolygonDraw::loadServerPositions(const string &title) {
    try {
        ifstream fin(title);
        string name, color;
        Vector2D position;

        servers.clear();

        if (fin.is_open()) {
            char line[256];
            string strLine,vectorStr;
            int posBeg,posEnd;
            while (!fin.eof()) {
                fin.getline(line,256);
                strLine=line;
                if (strLine.size()>1) {
                    posEnd = strLine.find(';');
                    name = strLine.substr(0, posEnd);
                    //cout << "name=" << name << endl;
                    posBeg = posEnd + 1;
                    posEnd = strLine.find(';', posBeg);
                    vectorStr = strLine.substr(posBeg, posEnd - posBeg);
                    //cout << "vectorStr=" << vectorStr << endl;
                    color = strLine.substr(posEnd + 1);
                    //cout << "color=" << color << endl;
                    posBeg = vectorStr.find('(');
                    posEnd = vectorStr.find(',');
                    position.x = stof(vectorStr.substr(posBeg + 1, posEnd - posBeg - 1));
                    posBeg = posEnd + 1;
                    posEnd = vectorStr.find(')', posBeg);
                    position.y = stof(vectorStr.substr(posBeg, posEnd - posBeg));
                    //cout << "position=" << position << endl;
                    servers.push_back(new ServerData(name, position, color));
                }
            }
        }
        fin.close();
    } catch (ifstream::failure e) {
        cerr << "Exception opening/reading/closing file" << endl;
    }
}

bool PolygonDraw::saveServerPositions(const string &title) {
    ofstream fout(title);

    if (fout.is_open()) {
        for (auto it:servers) {
            fout << (*it).name << ";" << (*it).position << ";" << (*it).colorName << "\n";
        }
    } else {
        return false;
    }
    return true;
}

void PolygonDraw::addDrone(const Vector2D &pos) {
    if (voronoi) {

        drones.emplace_back(new DroneData(pos));
        DroneData *d=drones[drones.size()-1];
        auto s=servers.begin();
        while (s!=servers.end() && !(*s)->droneIsOver(d->position)){
            s++;
        }
        assert(s!=servers.end());
        cout << "Initial server:" << (*s)->name << endl;
        (*s)->addDrone(d);
        d->ptrServer=*s;
        (*s)->printDrones();
    }
}

int main(int argc,char **argv) {
    PolygonDraw win("Servers and Drones", argc,argv);
    win.start();

    return 0;
}





