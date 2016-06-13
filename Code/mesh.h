#ifndef MESH_H
#define MESH_H

#include "Vertex.h"
#include <vector>
//#include "glm/glm.hpp"

/************************************************************
 * Class de triangle de base
 ************************************************************/
class Triangle {
public:
    inline Triangle () {
        v[0] = v[1] = v[2] = 0;
    }
    inline Triangle (const Triangle & t) {
        v[0] = t.v[0];
        v[1] = t.v[1];
        v[2] = t.v[2];
    }
    inline Triangle (unsigned int v0, unsigned int v1, unsigned int v2) {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
    }
    inline virtual ~Triangle () {}
    inline Triangle & operator= (const Triangle & t) {
        v[0] = t.v[0];
        v[1] = t.v[1];
        v[2] = t.v[2];
        return (*this);
    }
    unsigned int v[3];
};



/************************************************************
 * Class de maillage basique
 ************************************************************/
class Mesh {
public:
    Mesh();
    inline Mesh (const std::vector<Vertex> & v, const std::vector<Triangle> & t,
                 const std::vector<Vertex> & e) :
    vertices (v), triangles (t), texture (e){}
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Vertex> texture;

    bool loadMesh(const char * filename);
    void computeVertexNormals ();
    void centerAndScaleToUnit ();
    void draw();
    void drawSmooth();
	void drawWithColors(const std::vector<Vec3Df> & colors, int lose);
    void computeBoundingCube();
	int getClosestVertexIndex(const Vec3Df & origin, const Vec3Df & direction);

    //Bounding box information
    Vec3Df bbMinPos;
    float bbEdgeSize;
};

#endif // MESH_H
