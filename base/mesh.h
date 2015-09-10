
#pragma once

#include "object.h"
#include "vertex.h"
#include "bounding.h"
#include <glm/glm.hpp>

#define LN_MESH 1
#define IN_MESH 2

#define VERT_MESH 0x01
#define FACE_MESH 0x02
#define ATTR_MESH 0x04

class Mesh : public Object
 { private:
    int m_init;

    unsigned int numV;
    unsigned int numF;
    Vertex *vert;
    Face   *face;

    unsigned int vBuffer;
    unsigned int iBuffer;
    unsigned int aBuffer;
    unsigned int szVert;
    unsigned int szFace;

    unsigned char actual;

   public:
    AABBox bounding;

    Mesh();
    Mesh(const char *theName);
    ~Mesh();

    bool init();
    bool update();
    void clear();

    void render();

    void setVertex(Vertex *vertex, unsigned int sz);
    void setFace(Face *faces, unsigned int sz);

    void computeNormals();

    static Mesh* makeBox(float x, float y, float z);

    static Meta::Base Instance;
 };

