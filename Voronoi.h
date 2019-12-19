//
// Created by bpiranda on 10/12/2019.
//

#ifndef GEOMETRY_TRIANGULATION_GL_VORONOI_H
#define GEOMETRY_TRIANGULATION_GL_VORONOI_H
#include <vector>
#include "Mesh.h"
#include "Polygon.h"


class Voronoi {
    std::vector<MyPolygon*> polygons;
public:
    Voronoi(const Mesh &mesh);
    ~Voronoi();
    void draw();
    MyPolygon* findPolygon(const Vector2D &pos);
    void triangulatePolygons() {
        auto p=polygons.begin();
        while(p!=polygons.end()) {
            (*p)->triangulation();
            p++;
        }
    }
};


#endif //GEOMETRY_TRIANGULATION_GL_VORONOI_H
