
#include "render.h"
#include "unit.h"
#include "log.h"
#include "typevar.h"
#include "luastate.h"
#include "shader.h"
#include "frustrum.h"
//#include "fbuffer.h"
#include "scene.h"
#include "camera.h"
#include "material.h"
#include "mesh.h"
#include <GL/gl.h>

using namespace Render;

META_METHODS(Clear,
 METHOD(create, Clear::Create))
META_PROPERTY(Clear)
META_OBJECT(Clear, Render::Clear, &Component::Instance)

META_METHODS(ListDraw,
 METHOD(create, ListDraw::Create))
META_PROPERTY(ListDraw)
META_OBJECT(ListDraw, Render::ListDraw, &Component::Instance)

META_METHODS(MaterialDraw,
 METHOD(create, MaterialDraw::Create))
META_PROPERTY(MaterialDraw)
META_OBJECT(MaterialDraw, Render::MaterialDraw, &Component::Instance)

int ListDraw :: public_tab[] = {vCAMERA, vVECTOR_SCENE, 0};
int ListDraw :: privat_tab[] = {vLIST_DRAW, 0};

int MaterialDraw :: public_tab[] = {vCAMERA, 0};
int MaterialDraw :: privat_tab[] = {vLIST_DRAW, 0};

// ---------------------------------------------------------------

Clear :: Clear(unsigned int pt) : Component(pt)
 { m_update = (CUpdate) &Clear::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

void Clear :: doUpdate()
 { glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); }

// ----------------------------------------------------------------

ListDraw :: ListDraw(unsigned int pt) : Component(pt)
 { m_update = (CUpdate) &ListDraw::doUpdate;
   public_var = ListDraw::public_tab;
   privat_var = ListDraw::privat_tab;
   m_id = 0;
   metaClass = &Instance;
 }

void ListDraw :: linkVar(int def, void* data)
 { switch(def)
    { case vVECTOR_SCENE:
       mp_scenes = (std::vector<Scene::Render*>*)data;
       break;
      case vLIST_DRAW:
       mp_show = (Generic**)data;
       break;
      case vCAMERA:
       mpp_camera = (Camera::Render**)data;
       break;
    }
 }

void ListDraw :: doUpdate()
 { Generic* list = *mp_show;
   Generic* tmp;
   glm::mat4 mat(1.f);
   static int n;
   int i = 0;

   while(list)
    { tmp = list->next;
      pool.deallocate(list);
      list = tmp;
    }

   for(auto &it : *mp_scenes)
    i += it->isVisible(&list, pool, (*mpp_camera)->getFrustrum(), mat);

   *mp_show = list;

   if(i != n)
    { LOG_SPAM("Render::ListDraw: размер списка воспроизведения = %i", i);
      n = i;
    }

 }

// ---------------------------------------------------------------------

MaterialDraw :: MaterialDraw(unsigned int pt) : Component(pt)
 { public_var = MaterialDraw::public_tab;
   privat_var = MaterialDraw::privat_tab;
   m_update = (CUpdate)&MaterialDraw::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

MaterialDraw :: ~MaterialDraw()
 { }

void MaterialDraw :: linkVar(int def, void* data)
 { switch(def)
    { case vLIST_DRAW:
       mp_show = (Generic**)data;
       break;
      case vCAMERA:
       mpp_camera = (Camera::Render**)data;
       break;
      ////case _FBO_COLOR_:
       ////m_fbo = (FBuffer*)data;
       ////break; 
    }
 }

bool MaterialDraw :: init()
 { return true; }

void MaterialDraw :: clear()
 { }

void MaterialDraw :: doUpdate()
 { Generic* tmp = *mp_show;
   Shader* shd;

   while(tmp != nullptr)
    { shd = tmp->mat->bind();
      if(shd)
       { shd->setUniformMat("transform.PVM", (*mpp_camera)->getPVMat());
         shd->setUniformMat("transform.model", tmp->transform);
         tmp->mesh->render();
       }
      tmp = tmp->next;
    }
 }
