
#include "scene.h"
#include "unit.h"
#include "log.h"
#include "luastate.h"
#include "resmanager.h"
#include "bounding.h"
#include <glm/gtc/constants.hpp>

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
 METHOD(create, Control::Create),
 METHOD(move,   Control::move),
 METHOD(rotate,   Control::rotate))
META_PROPERTY(Control)
META_OBJECT(Control, Scene::Control, &Component::Instance)

META_METHODS(Build,
 METHOD(create, Build::Create))
META_PROPERTY(Build)
META_OBJECT(Build, Scene::Build, &Component::Instance)

int Render :: privat_tab[] = {vVECTOR_SEGMENT, vTRANSLATE, 0};
int Render :: public_tab[] = {vVECTOR_SCENE, 0};

int Behavior :: privat_tab[] = {vVECTOR_SEGMENT, vTRANSLATE, vOPERATION, 0};
int Behavior :: public_tab[] = {vVECTOR_SCENE, 0};

int Control :: privat_tab[] = {vOPERATION, 0};
int Control :: public_tab[] = {vLVM, 0};

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

Behavior :: Behavior(unsigned int pt) : Component(pt), m_trans(1.f), m_rot()
 { m_update = (CUpdate) &Behavior::doUpdate;
   privat_var = Behavior::privat_tab;
   public_var = Behavior::public_tab;
   m_id = 0;
   m_opt.flag = -1;

   max_speed = 0.5f;
   speed = 0.f;
   acceleration = 0.03f;

   acceleration_rot = 0.03f;

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
      case vOPERATION:
       mp_opt = (Operation*)data;
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
   if(mp_opt->flag == 1)
    { m_opt = *mp_opt;
      mp_opt->flag = 2;
      if(m_opt.type == 1)
       { start = m_trans[3][1];
         delta = 0.f;
         balance = 1.f / fabs(m_opt.param);
       }
      if(m_opt.type == 2)
       { delta = 0.f;
         m_rotate = glm::angleAxis(glm::half_pi<float>() * m_opt.param, glm::vec3(0.f, 1.f, 0.f));
       }
    }

   if(m_opt.flag == 0)
    { mp_opt->flag = 0;
      m_opt.flag = -1;
    }
   m_unit->unlock();
 }

void Behavior :: frame()
 { if(m_opt.flag == 1)
    switch(m_opt.type)
     { case 1: moveUp(); break;
       case 2: rotate(); break;
     }
 }

void Behavior :: moveUp()
 { if(delta < 0.5) speed += (double)acceleration * CONST_DELTA_TIME;
   else speed -= (double)acceleration * (double)CONST_DELTA_TIME;

   if(speed < acceleration * CONST_DELTA_TIME) speed = acceleration * CONST_DELTA_TIME;

   if(speed > max_speed) delta += max_speed * CONST_DELTA_TIME * balance;
   else delta += speed * (double)CONST_DELTA_TIME * balance;

   if(delta >= 1.0)
    { m_trans[3][1] = start + m_opt.param;
      speed = 0.0;
      delta = 0.0;
      m_opt.flag = 0;
      return;
    }
   m_trans[3][1] = start + m_opt.param * delta;
 }

void Behavior :: rotate()
 { glm::vec3 tmp(m_trans[3][0], m_trans[3][1], m_trans[3][2]);

   if(delta < 0.5) speed += (double)acceleration_rot * CONST_DELTA_TIME;
   else speed -= (double)acceleration_rot * (double)CONST_DELTA_TIME;

   if(speed < acceleration_rot * CONST_DELTA_TIME) speed = acceleration_rot * CONST_DELTA_TIME;

   delta += speed * (double)CONST_DELTA_TIME;
   if(delta >= 1.0)
    { m_rot = glm::slerp(m_rot, m_rotate, 1.f);
      m_trans = glm::mat4_cast(m_rot);
      m_trans[3][0] = tmp.x;
      m_trans[3][1] = tmp.y;
      m_trans[3][2] = tmp.z;
      speed = 0.0;
      delta = 0.0;
      m_opt.flag = 0;
      return;
    }
   m_trans = glm::mat4_cast(glm::slerp(m_rot, m_rotate, (float)delta));
   m_trans[3][0] = tmp.x;
   m_trans[3][1] = tmp.y;
   m_trans[3][2] = tmp.z;
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
    { case vLVM:
       lvm = (*(Lua::State**)data);
       break;
      case vOPERATION:
       mp_opt = (Operation*)data;
       mp_opt->flag = 0;
       break;
    }
 }

bool Control :: init()
 { regComp(*lvm);
   return true;
 }

void Control :: doUpdate()
 { m_unit->lock();
   if((mp_opt->flag == 0) && (m_opt.size() > 0))
    { *mp_opt = m_opt.front();
      m_opt.pop();
    }
   m_unit->unlock();
 }

void Control :: move(int flag, int dist)
 { Operation opt;
   opt.type = flag;
   opt.flag = 1;
   opt.param = dist;
   m_opt.push(opt);
   LOG_INFO("Control: Add action in list %i, %i ", flag, dist);
 }

void Control :: rotate(int flag, int param)
 { Operation opt;
   opt.type = flag;
   opt.flag = 1;
   opt.param = param;
   m_opt.push(opt);
   LOG_INFO("Control: Add action in list %i, %i ", flag, param);
 }

// ---------------------------------------------------------------

Build :: Build(Lua::Var& tab, unsigned int pt) : Component(pt)
 { Lua::Var pos = tab["pos"];
   if(pos == Lua::TAB)
    { m_pos.x = pos[1];
      m_pos.y = pos[2];
      m_pos.z = pos[3];
    }
   else m_pos = glm::vec3(0.f);

   m_update = (CUpdate) &Build::doUpdate;
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

   sgm = new Segment();

   shp = new Shape("tst-1");
   shp->setMaterial((Material*)mp_res->getObject("mt03.mat"));
   shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
   shp->setPos(glm::vec3(0.f, 0.f, 0.f));
   sgm->addShape(shp);

   //shp = new Shape("tst-2");
   //shp->setMaterial((Material*)mp_res->getObject("mt02.mat"));
   //shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
   //shp->setPos(glm::vec3(0.f, 0.f, 0.1f));
   //sgm->addShape(shp);

   //shp = new Shape("tst-3");
   //shp->setMaterial((Material*)mp_res->getObject("mt02.mat"));
   //shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
   //shp->setPos(glm::vec3(0.f, 0.f, 0.2f));
   //sgm->addShape(shp);

   //shp = new Shape("tst-4");
   //shp->setMaterial((Material*)mp_res->getObject("mt02.mat"));
   //shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
   //shp->setPos(glm::vec3(0.f, 0.1f, 0.f));
   //sgm->addShape(shp);

   //shp = new Shape("tst-5");
   //shp->setMaterial((Material*)mp_res->getObject("mt02.mat"));
   //shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
   //shp->setPos(glm::vec3(0.1f, 0.f, 0.f));
   //sgm->addShape(shp);

   //shp = new Shape("tst-6");
   //shp->setMaterial((Material*)mp_res->getObject("mt02.mat"));
   //shp->setMesh((Mesh*)mp_res->getObject("box_01.ms"));
   //shp->setPos(glm::vec3(-0.1f, 0.f, 0.f));
   //sgm->addShape(shp);

   mp_segment->push_back(sgm);

   glm::mat4 mt(1.f);
   mt[3][0] = m_pos.x;
   mt[3][1] = m_pos.y;
   mt[3][2] = m_pos.z;
   memcpy(mp_trans, &mt, sizeof(glm::mat4));

   return true;
 }

void Build :: doUpdate()
 { }
