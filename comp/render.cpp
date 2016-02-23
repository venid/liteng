
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

META_METHODS(Lists,
 METHOD(create, Lists::Create))
META_PROPERTY(Lists)
META_OBJECT(Lists, Render::Lists, &Component::Instance)

META_METHODS(Update,
 METHOD(create, Update::Create))
META_PROPERTY(Update)
META_OBJECT(Update, Render::Update, &Component::Instance)

META_METHODS(ListDraw,
 METHOD(create, ListDraw::Create))
META_PROPERTY(ListDraw)
META_OBJECT(ListDraw, Render::ListDraw, &Component::Instance)

META_METHODS(Geometry,
 METHOD(create, Geometry::Create))
META_PROPERTY(Geometry)
META_OBJECT(Geometry, Render::Geometry, &Component::Instance)

META_METHODS(Lighting,
 METHOD(create, Lighting::Create))
META_PROPERTY(Lighting)
META_OBJECT(Lighting, Render::Lighting, &Component::Instance)

META_METHODS(Compound,
 METHOD(create, Compound::Create))
META_PROPERTY(Compound)
META_OBJECT(Compound, Render::Compound, &Component::Instance)

int Lists :: public_tab[] = {vWORLD, vCAMERA, 0};
int Lists :: privat_tab[] = {vLIST_RENDER, 0};

int Update :: public_tab[] = {vCAMERA, 0};
int Update :: privat_tab[] = {vLIST_RENDER, vLIST_DRAW, 0};

int Geometry :: public_tab[] = {vCAMERA, 0};
int Geometry :: privat_tab[] = {vLIST_DRAW, 0};

int ListDraw :: public_tab[] = {vCAMERA, vVECTOR_SCENE, 0};
int ListDraw :: privat_tab[] = {vLIST_DRAW, 0};

int Compound :: public_tab[] = {vCAMERA, 0};
int Compound :: privat_tab[] = {vLIST_DRAW, 0};


// -----------------------------------------------------------------

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
      case vLIST_RENDER:
       mp_render = (Generic**)data;
       break;
      case vCAMERA:
       mpp_camera = (Camera::Translate**)data;
       break;
    }
 }

void Lists :: doUpdate()
 { Generic* list = nullptr;
   Generic* tmp;

   if(*mp_world) (*mp_world)->createList(&list, pool);

   m_unit->lock();
   tmp = *mp_render;
   *mp_render = list;
   m_unit->unlock();

   if(tmp)
    { list = tmp;
      while(list)
       { tmp = list->next;
         pool.deallocate(list);
         list = tmp;
       }
    }
 }

// ---------------------------------------------------------------

Update :: Update(unsigned int pt) : Component(pt)
 { m_update = (CUpdate) &Update::doUpdate;
   m_id = 0;
   public_var = Update::public_tab;
   privat_var = Update::privat_tab;
   metaClass = &Instance;
 }

void Update :: linkVar(int def, void* data)
 { switch(def)
    { case vLIST_RENDER:
       mp_render = (Generic**)data;
       break;
      case vLIST_DRAW:
       mp_show = (Generic**)data;
       break;
      case vCAMERA:
       mpp_camera = (Camera::Render**)data;
       break;
    }
 }

void Update :: doUpdate()
 { Generic* list;

   m_unit->lock();
   list = *mp_render;
   *mp_render = *mp_show;
   m_unit->unlock();

   *mp_show = list;
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
 }

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

  // for(auto &it : *mp_scenes)
  //  i += it->isVisible(&list, pool, (*mpp_camera)->getFrustrum(), mat);

   *mp_show = list;

   if(i != n)
    { LOG_SPAM("Render::ListDraw: размер списка воспроизведения = %i", i);
      n = i;
    }

 }

// ---------------------------------------------------------------------

Geometry :: Geometry(Lua::Var& tab, unsigned int pt) : Component(pt)
 { Object *obj = tab["program"];

   if(obj) m_shader = (Shader*)obj;
   else
    { m_shader = nullptr;
      LOG_WARNING("Render::Geometry: Not detected shader program.");
    }

   m_update = (CUpdate) &Geometry::doUpdate;
   public_var = Geometry::public_tab;
   privat_var = Geometry::privat_tab;
   m_id = 0;
   metaClass = &Instance;
 }

Geometry :: ~Geometry()
 { if(m_shader) m_shader->release(); }

void Geometry :: linkVar(int def, void* data)
 { switch(def)
    { case vLIST_DRAW:
       mp_show = (Generic**)data;
       break;
      case vCAMERA:
       mpp_camera = (Camera::Render**)data;
       break;
    }
 }

bool Geometry :: init()
 { if(m_shader) m_shader->init();
   return true;
 }

void Geometry :: clear()
 { if(m_shader) m_shader->clear(); }

void Geometry :: doUpdate()
 { Generic* tmp = *mp_show;

   while(tmp != nullptr)
    { if(!tmp->mesh->isInit()) tmp->mesh->init();
      if(m_shader)
       { m_shader->bind();
         m_shader->setUniformMat("transform.model", tmp->transform);
         m_shader->setUniformMat("transform.PVM", (*mpp_camera)->getPVMat());
         tmp->mesh->render();
       }
      tmp = tmp->next;
    }
 }

// ---------------------------------------------------------------------

Lighting :: Lighting(Lua::Var& tab, unsigned int pt) : Component(pt)
 { Object *obj = tab["program"];

   if(obj) m_shader = (Shader*)obj;
   else
    { m_shader = nullptr;
      LOG_WARNING("Render::Lighting: Not detected shader program.");
    }

   m_update = (CUpdate) &Lighting::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

void Lighting :: doUpdate()
 { }

// --------------------------------------------------------------------

Compound :: Compound(unsigned int pt) : Component(pt)
 { public_var = Compound::public_tab;
   privat_var = Compound::privat_tab;
   m_update = (CUpdate)&Compound::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

Compound :: ~Compound()
 { }

void Compound :: linkVar(int def, void* data)
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

bool Compound :: init()
 { return true; }

void Compound :: clear()
 { }

void Compound :: doUpdate()
 { //Generic* tmp = *mp_show;
   //Shader* shd;

   //while(tmp != nullptr)
    //{ shd = tmp->mat->bind();
      //if(shd)
       //{ shd->setUniformMat("transform.PVM", (*mpp_camera)->getPVMat());
         //shd->setUniformMat("transform.model", tmp->transform);
         //tmp->mesh->render();
       //}
      //tmp = tmp->next;
    //}
 }
