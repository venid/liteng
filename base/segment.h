
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

   public:
    Segment();
    ~Segment();

    bool init();
    void clear();

    void addShape(Shape* shape);

    int isVisible(Generic** head, MemoryPool<Generic> &pool,
                      Frustrum &frustrum, glm::mat4 &trans);

    static Meta::Base Instance;

 };
