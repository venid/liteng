
#pragma once

#include "object.h"
#include <glm/glm.hpp>
#include "vertex.h"
#include "MemoryPool.h"


class Frustrum;
class Material;
class Mesh;

class Shape : public Object
 { private:
    Mesh* m_mesh;
    Material* m_material;

    glm::mat4 m_trans;

   public:
    Shape(const char* Name);
    ~Shape();

    bool init();
    void clear();

    void setMaterial(Material* mat);
    void setMesh(Mesh* mesh);

    void setPos(const glm::vec3 &pos);

    int isVisible(Generic** head, MemoryPool<Generic> &pool,
                      Frustrum &frustrum, glm::mat4 &trans);

    static Meta::Base Instance;
 };
