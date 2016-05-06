
#pragma once

#include "object.h"
#include <vector>
#include <glm/glm.hpp>
#include "vertex.h"
#include "MemoryPool.h"

class Shape;
class Frustrum;

class Segment : public Object
 { private:
    std::vector<Shape*> m_shape;

    glm::mat4 m_trans;

   public:
    Segment();
    ~Segment();

    bool init();
    void clear();
    void update();

    void addShape(Shape* shape);

    static Meta::Base Instance;
 };
