
#pragma once

#include "object.h"
#include <glm/glm.hpp>
#include "vertex.h"
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

    Mesh* getMesh()         { return m_mesh; }
    Material* getMaterial() { return m_material; }

    void setPos(const glm::vec3 &pos);

    bool isVisible(Frustrum &frustrum, glm::mat4 &trans);

    static Meta::Base Instance;
 };
