
#include "segment.h"
#include "shape.h"
#include "frustrum.h"

META_METHODS(Segment)
META_PROPERTY(Segment)
META_OBJECT(Segment, Segment, &Object::Instance)

Segment :: Segment() : Object(), m_trans(1.f)
 {
   metaClass = &Instance;
 }

Segment :: ~Segment()
 { for(auto it : m_shape)
    it->release();

 }

bool Segment :: init()
 { for(auto it : m_shape)
    it->init();
   return true;
 }

void Segment :: clear()
 { for(auto it : m_shape)
    it->clear();
 }

void Segment :: update()
 { for(auto it : m_shape)
    it->update();
 }

void Segment :: addShape(Shape* shape)
 { m_shape.push_back(shape); }

int Segment :: isVisible(Generic** head, MemoryPool<Generic> &pool,
                         Frustrum &frustrum, glm::mat4 &trans)
 { int res = 0;
   glm::mat4 tmp = trans * m_trans;

   for(auto it : m_shape)
    res += it->isVisible(head, pool, frustrum, tmp);

   return res;
 }

