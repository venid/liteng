
#pragma once

#include "object.h"
#include "vertex.h"
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

    unsigned char numComp;
    unsigned int  typeGL;

    unsigned char actual;

    void tangentFace();
    void tangentVertex();

   public:
    Mesh();
    Mesh(const char *theName);
    ~Mesh();

    bool init();
    bool isInit() { return m_init; }
    bool update();
    void clear();

    void render();

    void setVertex(Vertex *vertex, unsigned int sz,
                   unsigned int type = GL_TRIANGLES);
    void setFace(Face *faces, unsigned int sz,
                 unsigned char num = 3);

    void getDimensions(glm::vec3 &vmin, glm::vec3 &vmax);

    void computeNormals();
    void computeTangents();
    void computeTextureCoord(float scale = 1.f);

    static Mesh* makeBox(float x, float y, float z);
    static Mesh* makeBoxFrame(glm::vec3& sz, glm::vec3& color);

    static Meta::Base Instance;
 };

