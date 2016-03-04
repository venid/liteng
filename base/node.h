
#pragma once

#include <vector>
#include "object.h"
#include "bounding.h"
#include "vertex.h"
#include "MemoryPool.h"

#define NODE_NOT 0
#define NODE_INTERNAL 1 //внутренний
#define NODE_EXTERNAL 2 //внешний

class Mesh;
class Shape;

class Node : public Object
 { public: //private:
    int m_type;
    AABBox m_box;

    Node* m_parent;
    std::vector<Node*> m_child;

    std::vector<Shape*> m_mesh;

    Mesh* m_frame;

   public:
    Node();

    bool add(Node* node, int ID);

    bool isVisible(Generic** head, MemoryPool<Generic> &pool,
                   glm::mat4 &trans);

    void imaging();
 };
