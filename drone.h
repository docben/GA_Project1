//
// Created by bpiranda on 16/12/2019.
//

#ifndef EXEMPLE_TEXTURE_GL_DRONE_H
#define EXEMPLE_TEXTURE_GL_DRONE_H

#include "Polygon.h"

const float dmax=32*3;
const float dmin=32*1.5;
const float Fmax=100.0;

class DroneData;

class ServerData {
public:
    string name;
    Vector2D position;
    string colorName;
    MyPolygon *ptrPolygon;
    vector<DroneData*> links2Drone;
    double surfaceRate;
    vector<ServerData*> neighbors;

    ServerData() { ptrPolygon= nullptr;};
    ServerData(const string &p_name,const Vector2D &p_position,const string &p_colorName):
            name(p_name),position(p_position),colorName(p_colorName)  { ptrPolygon= nullptr;};
    void addDrone(DroneData *ptr);
    void removeDrone(DroneData *ptr);
    void setDroneGoal(DroneData *drone);
    void addNeighbor(ServerData *neighbor);
    bool droneIsOver(const Vector2D &p) { return ptrPolygon->isInsideTriangles(p.x,p.y); };
    void printDrones();
};


class DroneData {
public:
    int id;
    Vector2D position;
    Vector2D V;
    Vector2D goal;
    Vector2D sumF;
    ServerData *ptrServer;
    DroneData() {
        ptrServer= nullptr;
    }

    DroneData(const Vector2D& p_position):position(p_position) {
        static int memid=1;

        id=memid++;
        ptrServer= nullptr;
    }
    void glDraw();
    void updateSpeed(double dt);
};


#endif //EXEMPLE_TEXTURE_GL_DRONE_H
