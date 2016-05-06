#include "draw.h"
#include "unit.h"
#include "log.h"
#include "typevar.h"
#include "luastate.h"
#include "shader.h"
#include "mesh.h"
#include "camera.h"
#include "scene.h"
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace Interface;

META_METHODS(Lists,
 METHOD(create, Lists::Create))
META_PROPERTY(Lists)
META_OBJECT(Lists, Interface::Lists, &Component::Instance)

META_METHODS(Border,
 METHOD(create, Border::Create))
META_PROPERTY(Border)
META_OBJECT(Border, Interface::Border, &Component::Instance)

int Lists :: public_tab[] = {vWORLD, 0};
int Lists :: privat_tab[] = {vLIST_BORDER, 0};

int Border :: public_tab[] = {vCAMERA, 0};
int Border :: privat_tab[] = {vLIST_BORDER, 0};

// --------------------------------------------------------------------------

Lists :: Lists(unsigned int pt) : Component(pt)
 { m_update = (CUpdate) &Lists::doUpdate;
   m_id = 0;
   public_var = Lists::public_tab;
   privat_var = Lists::privat_tab;
   metaClass = &Instance;
 }

void Lists :: linkVar(int def, void* data)
 { switch(def)
    { case vWORLD:
       mp_world = (Scene::World**)data;
       break;
      case vLIST_BORDER:
       mpp_border = (Scope**)data;
       *mpp_border = nullptr;
       break;
    }
 }

void Lists :: doUpdate()
 { Scope* list;
   Scope* tmp;
   glm::mat4 trans(1.f);

   mp_border = nullptr;
   if(*mp_world) createList((*mp_world)->getNode(), trans);

   m_unit->lock();
   tmp = *mpp_border;
   *mpp_border = mp_border;
   m_unit->unlock();

   if(tmp)
    { list = tmp;
      while(list)
       { tmp = list->next;
         m_pool.deallocate(list);
         list = tmp;
       }
    }
 }

void Lists :: createList(Node* node, glm::mat4 &trans)
 { glm::vec3 scale(node->m_box.getScale());
   glm::vec3 pos(node->m_box.getPos());
   glm::mat4 tmp = trans;

   tmp[3][0] += pos.x;
   tmp[3][1] += pos.y;
   tmp[3][2] += pos.z;

   if(node->m_type == NODE_EXTERNAL)
    { Scope* scope = m_pool.allocate();
      scope->id = node->getId();
      scope->transform = glm::scale(tmp, scale * 2.f);
      scope->color = glm::vec4(0.2f, 1.f, 0.2f, 1.f);
      scope->next = mp_border;
      mp_border = scope;
    }

   for(auto &it : node->m_child)
    createList(it, tmp);

 }

// --------------------------------------------------------------------

Border :: Border(Lua::Var& tab, unsigned int pt) : Component(pt)
 { Object *obj = tab["program"];

   if(obj) m_shader = (Shader*)obj;
   else
    { m_shader = nullptr;
      LOG_WARNING("Interface::Border: Not detected shader program.");
    }

   glm::vec3 sz(1.f);
   m_box = Mesh::makeBoxFrame(sz);

   public_var = Border::public_tab;
   privat_var = Border::privat_tab;
   m_update = (CUpdate)&Border::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

Border :: ~Border()
 { if(m_shader) m_shader->release(); }

void Border :: linkVar(int def, void* data)
 { switch(def)
    { case vCAMERA:
       mpp_camera = (Camera::Render**)data;
       break;
      case vLIST_BORDER:
       mpp_border = (Scope**)data;
       break;
    }
 }

bool Border :: init()
 { if(m_shader) m_shader->init();
   m_box->init();
   return true;
 }

void Border :: clear()
 { if(m_shader) m_shader->clear();
   m_box->clear();
 }

void Border :: doUpdate()
 { Scope* list;

   m_unit->lock();
   list = *mpp_border;
   *mpp_border = mp_border;
   m_unit->unlock();

   mp_border = list;

   while(list != nullptr)
    { if(m_shader)
       { m_shader->bind();
         m_shader->setUniformMat("transform.model", list->transform);
         m_shader->setUniformMat("transform.PVM", (*mpp_camera)->getPVMat());
         m_shader->setUniformVec("material.diffuse", list->color);
         m_box->render();
       }
      list = list->next;
    }
 }
