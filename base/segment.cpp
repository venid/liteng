
#include "segment.h"
#include "shape.h"
#include "frustrum.h"

META_METHODS(Segment)
META_PROPERTY(Segment)
META_OBJECT(Segment, Segment, &Object::Instance)

Segment :: Segment() : Object()
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

void Segment :: addShape(Shape* shape)
 { m_shape.push_back(shape); }

int Segment :: isVisible(Generic** head, MemoryPool<Generic> &pool,
                         Frustrum &frustrum, glm::mat4 &trans)
 { int res = 0;

   for(auto it : m_shape)
    res += it->isVisible(head, pool, frustrum, trans);

   return res;
 }

