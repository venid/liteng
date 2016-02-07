#include "physics.h"
#include "object.h"
#include "defs.h"
#include "manager.h"
#include "log.h"
#include "component.h"
#include "luastate.h"
#include "typevar.h"

META_METHODS(Physics,
 METHOD(create, Physics::Create))
META_PROPERTY(Physics)
META_OBJECT(Physics, Physics, &Module::Instance)

/// -----------------------------------------------------------------------------------

Physics :: Physics(const char* Name) : Module(Name)
 { do_update = (MUpdate)&Physics::init_update;
   metaClass = &Instance;
 }

void Physics :: set_var()
 { mp_delta = new double;
   pool.emplace(vDELTA_TIME, Link{1, mp_delta});
 }

void Physics :: connect()
 { Manager::sendMessage(MSG_CONNECT, this, MSG_ADD_UNIT); }

bool Physics :: init(Lua::State &lua)
 {
   LOG_INFO("%s: first init.", getName());
   return true;
 }

bool Physics :: msg_processing()
 { int tMsg, param;
   Object* pobj;

   while(Manager::getMessage(getNumQueue(), tMsg, pobj, param))
    { switch(tMsg)
       { case MSG_ADD_UNIT:
          addComp((Unit*)pobj);
          pobj->release();
          break;
         case MSG_FINISH:
          do_update = (MUpdate)&Physics::clear_update;
          return false;
       }
    }
   return true;
 }

int Physics :: init_update(double tm)
 { Manager :: sendMessage(MSG_ACTIVE, nullptr, getNumQueue());

   set_var();
   LOG_INFO("%s: second init.", getName());
   do_update = (MUpdate)&Physics::run_update;
   return 1;
 }

int Physics :: clear_update(double tm)
 { delComp();
   pool[vDELTA_TIME].refCount --;
   del_var();
   LOG_INFO("%s: clear.", getName());
   do_update = &Module::empty_update;
   return 0;
 }

int Physics :: run_update(double tm)
 { *mp_delta = tm;

   if(msg_processing())
    { for(auto &it : components)
       it.second->update();
    }
   return 1;
 }
