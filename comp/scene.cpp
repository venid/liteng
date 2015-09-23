
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

META_METHODS(Behavior,
 METHOD(create, Behavior::Create))
META_PROPERTY(Behavior)
META_OBJECT(Behavior, Scene::Behavior, &Component::Instance)

META_METHODS(Control,
 METHOD(create, Control::Create))
META_PROPERTY(Control)
META_OBJECT(Control, Scene::Control, &Component::Instance)

META_METHODS(Build,
 METHOD(create, Build::Create))
META_PROPERTY(Build)
META_OBJECT(Build, Scene::Build, &Component::Instance)

int Render :: privat_tab[] = {vVECTOR_SEGMENT, vTRANSLATE, 0};
int Render :: public_tab[] = {vVECTOR_SCENE, 0};

int Behavior :: privat_tab[] = {vVECTOR_SEGMENT, vTRANSLATE, 0};
int Behavior :: public_tab[] = {vVECTOR_SCENE, 0};

int Control :: privat_tab[] = {0};
int Control :: public_tab[] = {0};

int Build :: privat_tab[] = {vVECTOR_SEGMENT, vTRANSLATE, 0};
int Build :: public_tab[] = {vRES_MANAGER, 0};

// ---------------------------------------------------------------

Render :: Render(unsigned int pt) : Component(pt), m_trans(1.f)
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
      case vTRANSLATE:
       mp_trans = (glm::mat4*)data;
       break;
    }
 }

void Render :: doUpdate()
 { m_unit->lock();
   memcpy(&m_trans, mp_trans, sizeof(glm::mat4));
   m_unit->unlock();

   for(auto it = mp_segment->begin(); it != mp_segment->end(); it++)
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
   glm::mat4 tmp = trans * m_trans;

   m_unit->lock();
   for(auto it = mp_segment->begin(); it != mp_segment->end(); it++)
    res += (*it)->isVisible(head, pool, frustrum, tmp);
   m_unit->unlock();

   return res;
 }

// ---------------------------------------------------------------

Behavior :: Behavior(unsigned int pt) : Component(pt), m_trans(1.f)
 { m_update = (CUpdate) &Behavior::doUpdate;
   privat_var = Behavior::privat_tab;
   public_var = Behavior::public_tab;
   m_id = 0;
   metaClass = &Instance;
 }

void Behavior :: linkVar(int def, void* data)
 { switch(def)
    { case vVECTOR_SEGMENT:
       mp_segment = (std::vector<Segment*>*)data;
       break;
      case vVECTOR_SCENE:
       mp_scenes = (std::vector<Object*>*)data;
       break;
      case vTRANSLATE:
       mp_trans = (glm::mat4*)data;
       break;
    }
 }

bool Behavior :: init()
 { m_unit->lock();
   memcpy(&m_trans, mp_trans, sizeof(glm::mat4));
   m_unit->unlock();

   mp_scenes->push_back(this->retain());
   return true;
 }

void Behavior :: clear()
 { std::vector<Object*>::iterator itr;

   for(itr = mp_scenes->begin(); itr != mp_scenes->end(); itr++)
    if (*itr == this)
     { (*itr)->release();
       break;
     }
   mp_scenes->erase(itr);
 }

void Behavior :: doUpdate()
 { m_unit->lock();
   memcpy(mp_trans, &m_trans, sizeof(glm::mat4));
   m_unit->unlock();
 }

void Behavior :: move()
 { glm::vec3 tmp(0.02f, 0.f, 0.015f);

   tmp *= CONST_DELTA_TIME;
   m_trans[3][0] += tmp.x;
   m_trans[3][1] += tmp.y;
   m_trans[3][2] += tmp.z;
 }

// ---------------------------------------------------------------

Control :: Control(unsigned int pt) : Component(pt)
 { m_update = (CUpdate) &Control::doUpdate;
   privat_var = Control::privat_tab;
   public_var = Control::public_tab;
   m_id = 0;
   metaClass = &Instance;
 }

void Control :: linkVar(int def, void* data)
 { switch(def)
    { 
    }
 }

bool Control :: init()
 {
   return true;
 }

void Control :: doUpdate()
 { }

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
      case vTRANSLATE:
       mp_trans = (glm::mat4*)data;
       break;
    }
 }

bool Build :: init()
 { Shape* shp;
   Segment* sgm;
   char name[16];
   int n = 0;

   sgm = new Segment();

   for(int i = 0; i < 5; i++)
    for(int j = 0; j < 5; j++)
     { sprintf(name, "test%i.sh", n);
       shp = new Shape(name);
       shp->setMaterial((Material*)mp_res->getObject("mt02.mat"));
       shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
       shp->setPos(glm::vec3(-0.4f + i * 0.2f, 0.f, -0.4f + j * 0.2f));
       sgm->addShape(shp);
       n++;
     }

   mp_segment->push_back(sgm);

   glm::mat4 mt(1.f);
   mt[3][0] = 1.f;
   mt[3][1] = 0.f;
   mt[3][2] = -2.f;
   memcpy(mp_trans, &mt, sizeof(glm::mat4));

   return true;
 }

void Build :: doUpdate()
 { }
