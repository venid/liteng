
#include "shape.h"
#include "log.h"
#include "mesh.h"
#include "material.h"
#include "bounding.h"

META_METHODS(Shape)
META_PROPERTY(Shape)
META_OBJECT(Shape, Shape, &Object::Instance)

Shape :: Shape() : Object()
 { id = Object::genID();
   m_visible = true;
   m_material = nullptr;
   m_mesh = nullptr;
   metaClass = &Instance;
 }

Shape :: Shape(const char* Name) : Object(Name, Object::genID())
 { m_visible = true;
   m_material = nullptr;
   m_mesh = nullptr;
   metaClass = &Instance;
 }

Shape :: ~Shape()
 { if(m_material != nullptr) m_material->release();
   if(m_mesh != nullptr) m_mesh->release();
 }

bool Shape :: init()
 { if(m_material != nullptr) m_material->init();
   if(m_mesh != nullptr) m_mesh->init();
  // m_visible = true;
   return true;
 }

void Shape :: clear()
 { if(m_material != nullptr) m_material->clear();
   if(m_mesh != nullptr) m_mesh->clear();
   m_visible = false;
 }

void Shape :: update()
 { if(m_mesh != nullptr) m_mesh->update(); }

void Shape :: setMaterial(Material* mat)
 { if(m_material != nullptr) m_material->release();

   m_material = mat;
 }

void Shape :: setMesh(Mesh* mesh)
 { glm::vec3 vmin, vmax, tmp;

   if(m_mesh != nullptr) m_mesh->release();

   m_mesh = mesh;
   m_mesh->getDimensions(vmin, vmax);
   tmp = glm::abs((vmax - vmin) * 0.5f);
   bounding.setScale(tmp);
   bounding.setPos(vmin + tmp);
 }

void Shape :: setPos(const glm::vec3 &pos)
 { m_trans[3][0] = pos.x;
   m_trans[3][1] = pos.y;
   m_trans[3][2] = pos.z;
 }

int Shape :: isVisible(Generic** head, MemoryPool<Generic> &pool,
                       Frustrum &frustrum, glm::mat4 &trans)
 { if(m_visible)
    { OBBox box(bounding);
      glm::mat4 tmp = m_trans * trans;

      box.move(tmp);
      if(box.intersects(frustrum))
       { Generic* show = pool.allocate();
         show->transform = tmp;
         show->next = *head;
         show->mesh = m_mesh;
         show->mat = m_material;
         show->id = getId();
         *head = show;
         return 1;
       }
    }
   return 0;
 }
