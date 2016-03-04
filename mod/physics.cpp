#include "physics.h"
#include "object.h"
#include "defs.h"
#include "manager.h"
#include "log.h"
#include "component.h"
#include "luastate.h"
#include "typevar.h"

META_METHODS(Physics,
 METHOD(create, Physics::Create),
 METHOD(clear, Physics::clear),
 METHOD(add, Physics::add))
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

void Physics :: connectMsg()
 { addMsg(MSG_FINISH, this, "clear");
   addMsg(MSG_ADD_UNIT, this, "add");
 }

bool Physics :: init(Lua::State &lua)
 {
   LOG_INFO("%s: first init.", getName());
   return true;
 }

int Physics :: init_update(double tm)
 { Manager :: sendMessage(MSG_ACTIVE, nullptr, getNumQueue());

   set_var();
   LOG_INFO("%s: second init.", getName());
   do_update = (MUpdate)&Physics::run_update;
   return 1;
 }

int Physics :: run_update(double tm)
 { *mp_delta = tm;

   for(auto &it : components)
       it.second->update();

   return 1;
 }

void Physics :: clear(Object* obj, int param)
 { delComp();
   pool[vDELTA_TIME].refCount --;
   del_var();
   LOG_INFO("%s: clear.", getName());
   do_update = &Module::empty_update;
 }

void Physics :: add(Object* pobj, int param)
 { addComp((Unit*)pobj);
   pobj->release();
 }
