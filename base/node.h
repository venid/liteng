
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

class Node : public Object
 { public: //private:
    int m_type;
    OBBox m_box;

    Node* m_parent;
    std::vector<Node*> m_child;

    Mesh* m_mesh;

   public:
    Node();

    bool add(Node* node, int ID);

    bool isVisible(Generic** head, MemoryPool<Generic> &pool,
                   glm::mat4 &trans);

    void imaging();
 };
