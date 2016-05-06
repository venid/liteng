
#include "node.h"
#include "log.h"

#include "mesh.h"

Node :: Node() : Object(), m_box()
 { m_type = NODE_NOT;
   m_parent = nullptr;
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

bool Node :: isVisible(Frustrum &frustrum, glm::mat4 &trans)
 { //glm::vec3 pos(m_box.getPos());
   //glm::mat4 tmp = trans;

   //tmp[3][0] += pos.x;
   //tmp[3][1] += pos.y;
   //tmp[3][2] += pos.z;

   //Generic* show = pool.allocate();
   //show->transform = tmp;
   //show->next = *head;
   //show->mesh = m_frame;
   //show->mat = nullptr;
   //show->id = getId();
   //*head = show;

   //for(auto &it : m_child)
    //it->isVisible(head, pool, tmp);

   return true;
 }
