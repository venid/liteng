
#include "dynamics.h"
#include "unit.h"
#include "log.h"
#include "typevar.h"
#include "luastate.h"
#include "scene.h"

using namespace Dynamics;

META_METHODS(Scenes,
 METHOD(create, Scenes::Create))
META_PROPERTY(Scenes)
META_OBJECT(Scenes, Dynamics::Scenes, &Component::Instance)

int Scenes :: public_tab[] = {vDELTA_TIME, vVECTOR_SCENE, 0};
int Scenes :: privat_tab[] = {0};

// --------------------------------------------------------------------------

Scenes :: Scenes(unsigned int pt) : Component(pt)
 { m_rest = 0.0;
   m_update = (CUpdate) &Scenes::doUpdate;
   public_var = Scenes::public_tab;
   privat_var = Scenes::privat_tab;
   m_id = 0;
   metaClass = &Instance;
 }

void Scenes :: linkVar(int def, void* data)
 { switch(def)
    { case vVECTOR_SCENE:
       mp_scenes = (std::vector<Scene::Behavior*>*)data;
       break;
      //case vLIST_DRAW:
       //mp_show = (Generic**)data;
       //break;
      case vDELTA_TIME:
       mp_delta = (double*)data;
       break;
    }
 }

void Scenes :: doUpdate()
 { int n = 0;
   double time = *mp_delta + m_rest;

   while(time > CONST_DELTA_TIME)
    { time -= CONST_DELTA_TIME;

      for(auto &it : *mp_scenes)
       it->frame();

      n++;
      if(n == MAX_ITERATION)
       { time = 0.0;
         break;
       }
    }
   m_rest = time;
 }

