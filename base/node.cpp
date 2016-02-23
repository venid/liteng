
#include "node.h"
#include "log.h"

#include "mesh.h"

Node :: Node() : Object(), m_box()
 { m_type = NODE_NOT;
   m_parent = nullptr;
   m_mesh = nullptr;
 }

bool Node :: add(Node* node, int ID)
 { if(node)
    { if(id == ID)
       { node->m_parent = this;
         m_child.push_back(node);
         return true;
       }
      else
        for(auto &it : m_child)
         if(it->add(node, ID)) return true;
    }
   return false;
 }

void Node :: imaging()
 { glm::vec3 sz = m_box.getScale();
   glm::vec3 cl;

   sz *= 2.f;
   if(m_type == NODE_EXTERNAL) cl = glm::vec3(0.5f, 1.f, 0.5f);
   else cl = glm::vec3(1.f, 0.5f, 0.5f);
   m_mesh = Mesh::makeBoxFrame(sz, cl);
 }

bool Node :: isVisible(Generic** head, MemoryPool<Generic> &pool,
                       glm::mat4 &trans)
 { glm::mat4 tmp = trans * m_box.getTransform();

   Generic* show = pool.allocate();
   show->transform = tmp;
   show->next = *head;
   show->mesh = m_mesh;
   show->mat = nullptr;
   show->id = getId();
   *head = show;

   for(auto &it : m_child)
    it->isVisible(head, pool, tmp);

   return true;
 }
