
#include "scene.h"
#include "unit.h"
#include "log.h"
#include "typevar.h"
#include "luastate.h"
#include "resmanager.h"
#include "bounding.h"

#include "segment.h"
#include "shape.h"
#include "mesh.h"
#include "material.h"

using namespace Scene;

META_METHODS(Render,
 METHOD(create, Render::Create))
META_PROPERTY(Render)
META_OBJECT(Render, Scene::Render, &Component::Instance)

META_METHODS(Build,
 METHOD(create, Build::Create))
META_PROPERTY(Build)
META_OBJECT(Build, Scene::Build, &Component::Instance)

int Render :: privat_tab[] = {vVECTOR_SEGMENT, 0};
int Render :: public_tab[] = {vVECTOR_SCENE, 0};

int Build :: privat_tab[] = {vVECTOR_SEGMENT, 0};
int Build :: public_tab[] = {vRES_MANAGER, 0};

// ---------------------------------------------------------------

Render :: Render(unsigned int pt) : Component(pt)
 { m_update = (CUpdate) &Render::doUpdate;
   privat_var = Render::privat_tab;
   public_var = Render::public_tab;
   m_id = 0;
   metaClass = &Instance;
 }

void Render :: linkVar(int def, void* data)
 { switch(def)
    { case vVECTOR_SEGMENT:
       mp_segment = (std::vector<Segment*>*)data;
       break;
      case vVECTOR_SCENE:
       mp_scenes = (std::vector<Object*>*)data;
       break;
    }
 }

void Render :: doUpdate()
 { for(auto it = mp_segment->begin(); it != mp_segment->end(); it++)
    (*it)->update();
 }

bool Render :: init()
 { m_unit->lock();
   for(auto it = mp_segment->begin(); it != mp_segment->end(); it++)
    { (*it)->init(); }
   m_unit->unlock();
   mp_scenes->push_back(this->retain());
   return true;
 }

void Render :: clear()
 { std::vector<Object*>::iterator itr;

   m_unit->lock();
   for(auto it = mp_segment->begin(); it != mp_segment->end(); it++)
    { (*it)->clear(); }
   m_unit->unlock();

   for(itr = mp_scenes->begin(); itr != mp_scenes->end(); itr++)
    if (*itr == this)
     { (*itr)->release();
       break;
     }
   mp_scenes->erase(itr);
 }

int Render :: isVisible(Generic** head, MemoryPool<Generic> &pool,
                        Frustrum &frustrum, glm::mat4 &trans)
 { int res = 0;

   m_unit->lock();
   for(auto it = mp_segment->begin(); it != mp_segment->end(); it++)
    res += (*it)->isVisible(head, pool, frustrum, trans);
   m_unit->unlock();

   return res;
 }

// ---------------------------------------------------------------

Build :: Build(unsigned int pt) : Component(pt)
 { m_update = (CUpdate) &Build::doUpdate;
   privat_var = Build::privat_tab;
   public_var = Build::public_tab;
   m_id = 0;
   metaClass = &Instance;
 }

void Build :: linkVar(int def, void* data)
 { switch(def)
    { case vVECTOR_SEGMENT:
       mp_segment = (std::vector<Segment*>*)data;
       break;
      case vRES_MANAGER:
       mp_res = (*(ResManager**)data);
       break;
    }
 }

bool Build :: init()
 { Shape* shp;
   Segment* sgm;
   char name[16];
   int n = 0;

   sgm = new Segment();

   for(int i = 0; i < 10; i++)
    for(int j = 0; j < 10; j++)
     { sprintf(name, "test%i.sh", n);
       shp = new Shape(name);
       shp->setMaterial((Material*)mp_res->getObject("mt01.mat"));
       shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
       shp->setPos(glm::vec3(-0.9f + i * 0.2f, 0.f, -0.9f + j * 0.2f));
       sgm->addShape(shp);
       n++;
     }

   shp = new Shape("test_up.sh");
   shp->setMaterial((Material*)mp_res->getObject("mt02.mat"));
   shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
   shp->setPos(glm::vec3(0.f, 0.5f, 0.f));
   sgm->addShape(shp);

   shp = new Shape("test_down.sh");
   shp->setMaterial((Material*)mp_res->getObject("mt02.mat"));
   shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
   shp->setPos(glm::vec3(0.f, -0.5f, 0.f));
   sgm->addShape(shp);

   mp_segment->push_back(sgm);
   return true;
 }

void Build :: doUpdate()
 { }
