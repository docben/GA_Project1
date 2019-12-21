//
// Created by bpiranda on 16/12/2019.
//

#include <glutWindow.h>
#include "Drone.h"
#include <algorithm>

void DroneData::updateSpeed(double dt) {
    if (ptrServer!= nullptr) {
        Vector2D a = (goal-position)+sumF;
        V = 0.95 * V + ((float) (dt)) * a;
        position = position + ((float) (dt)) * V;
    }
}

void DroneData::glDraw() {
    if (ptrServer!= nullptr) {
        glColor4fv(&BLACK[0]);
        glBegin(GL_LINES);
        glVertex2f(0,0);
        glVertex2f(ptrServer->position.x-position.x,ptrServer->position.y-position.y);
        glEnd();
    }
    if (!V.isNull()) {
        Vector2D X=V;
        X.normalize();
        double a,cs,sn;
        glLineWidth(3);
        glBegin(GL_LINES);
        glVertex2f(0,0);
        glVertex2f(V.x,V.y);

        for (int i=0; i<20; i++) {
            a = -0.5*M_PI+i*M_PI/20.0;
            cs = dmax*cos(a);
            sn = dmax*sin(a);
            glVertex2d(cs*X.x-sn*X.y,cs*X.y+sn*X.x);
        }
        a = dmax;
        for (int i=0; i<10; i++) {
            glVertex2d(-a*X.y,a*X.x);
            a-=dmax/5.0;
        }
        glEnd();
        glLineWidth(1);

    }
}

void ServerData::addDrone(DroneData *ptr) {
    links2Drone.push_back(ptr);
    setDroneGoal(ptr);
    cout << "Drone " << ptr->id << " added to " << name << endl;
    cout << "verif=" << links2Drone[links2Drone.size()-1]->id << endl;
}

void ServerData::removeDrone(DroneData *ptr) {
    auto it = links2Drone.begin();
    while (it!=links2Drone.end() && (*it)->id!=ptr->id) {
        it++;
    }
    if (it!=links2Drone.end()) {
        links2Drone.erase(it);
        cout << "Drone " << ptr->id << " removed from " << name << endl;
    } else {
        cerr << "Drone " << ptr->id << " not found in " << name << endl;
        printDrones();
    }
}

void ServerData::printDrones() {
    cout << "size=" << links2Drone.size() << endl;
    for (auto d:links2Drone) {
        cout << d->id << " ";
    }
    cout << endl;
}

void ServerData::addNeighbor(ServerData *neighbor) {
    neighbors.push_back(neighbor);
}

void ServerData::setDroneGoal(DroneData *drone) {
    float serverWeight=(links2Drone.size())/surfaceRate;
    //cout << "Server " << name << " weight=" << serverWeight << " (" << links2Drone.size() << " drones)" << endl;
    sort(neighbors.begin(), neighbors.end(), [](ServerData *a, ServerData *b) {
        return (a->links2Drone.size() + 1) / a->surfaceRate < (b->links2Drone.size() + 1) / b->surfaceRate;
    });
    int ind=drone->ptrServer==neighbors[0]?1:0;
    //cout << "Best " << neighbors[0]->name << " weight=" << (neighbors[0]->links2Drone.size() + 1) / neighbors[0]->surfaceRate << " (" << neighbors[0]->links2Drone.size() << " drones)" << endl;
    //cout << "Next " << neighbors[1]->name << " weight=" << (neighbors[1]->links2Drone.size() + 1) / neighbors[1]->surfaceRate << " (" << neighbors[1]->links2Drone.size() << " drones)" << endl;

    float neighborWeight=(neighbors[ind]->links2Drone.size() + 1) / neighbors[ind]->surfaceRate;
    if(serverWeight<neighborWeight) {
        drone->goal = position;
    } else {
        drone->goal = neighbors[ind]->position;
    }
}