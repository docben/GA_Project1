//
// Created by bpiranda on 10/12/2019.
//

#ifndef GEOMETRY_TRIANGULATION_GL_VORONOI_H
#define GEOMETRY_TRIANGULATION_GL_VORONOI_H
#include <vector>
#include "Mesh.h"
#include "Polygon.h"


class Voronoi {
    //std::vector<Vector2D*> vertices;
    std::vector<MyPolygon*> polygons;
public:
    Voronoi(const Mesh &mesh);
    ~Voronoi();
    void draw();
};


#endif //GEOMETRY_TRIANGULATION_GL_VORONOI_H
