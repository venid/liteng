
#pragma once

#include "object.h"
#include <glm/glm.hpp>
#include "vertex.h"
#include "MemoryPool.h"
#include "bounding.h"


class Frustrum;
class Material;
class Mesh;

class Shape : public Object
 { private:
    bool m_visible;
    Mesh* m_mesh;
    Material* m_material;
    AABBox bounding;

    glm::mat4 m_trans;

   public:
    Shape();
    Shape(const char* Name);
    ~Shape();

    bool init();
    void clear();
    void update();

    void setMaterial(Material* mat);
    void setMesh(Mesh* mesh);
    void setVisible(bool param) { m_visible = param; }

    void setPos(const glm::vec3 &pos);

    int isVisible(Generic** head, MemoryPool<Generic> &pool,
                      Frustrum &frustrum, glm::mat4 &trans);

    static Meta::Base Instance;
 };
