
#include "factory.h"
#include "object.h"
#include "data.h"
#include "defs.h"
#include "typevar.h"
#include "manager.h"
#include "log.h"
#include "resmanager.h"
#include "filesystem.h"
#include "zipfilesystem.h"
#include "unit.h"
#include "component.h"
#include <algorithm>

#include "pdecoder.h"
#include "matdecoder.h"
#include "meshdecoder.h"

META_METHODS(Factory,
 METHOD(create, Factory::Create))
META_PROPERTY(Factory)
META_OBJECT(Factory, Factory, &Module::Instance)

unsigned int Factory :: messages_list[] = {MSG_FINISH, MSG_TEST_5, MSG_MAKE_UNIT, MSG_ADD_UNIT, 0};
ResManager* Factory :: resManager = nullptr;

// ------------------------------------------------------------------------------------------------

Factory :: Factory(const char* Name) : Module(Name), lvm()
 { resManager = new ResManager(lvm.vm);
   do_update = (MUpdate) &Factory::init_update;
   tabMessages = messages_list;
   metaClass = &Instance;
 }

Factory :: ~Factory()
 { if(resManager) delete resManager; }

void Factory :: set_var()
 { pool.emplace(vRES_MANAGER, Link{1, setPointer(resManager)}); }

bool Factory :: msg_processing()
 { int tMsg, param;
   Object* pobj;

   while(Manager::getMessage(getNumQueue(), tMsg, pobj, param))
    { switch(tMsg)
       { case MSG_ADD_UNIT:
          if(((Object*)pobj)->release() == false)
           entity.push_back((Unit*)pobj);
          else LOG_ERROR("%s: неплановое удаление unit", getName());
          break;
         case MSG_MAKE_UNIT:
          addComp((Unit*)pobj);
          pobj->release();
          ////LOG_SPAM("%s: количество ссылок на unit %i", getName(), ((PObject)pobj)getRefCount());
          Manager::sendMessage(MSG_ADD_UNIT, pobj, 0);
          LOG_SPAM("%s: Message MSG_MAKE_UNIT", getName());
          break;
         case MSG_TEST_5:
          entity.erase(std::remove_if(entity.begin(), entity.end(),
                      [](Unit* un)->bool {return un->remove();}),
                      entity.end());
          LOG_SPAM("%s: Message MSG_TEST_5", getName());
          //for(auto &it : components)
           //it.second->update();
          break;
         case MSG_FINISH:
          do_update = (MUpdate) &Factory::clear_update;
          return false;
       }
    }
   return true;
 }

bool Factory :: init(Lua::State &lua)
 { std::string str;
   int n;
   Lua::Var tab = lua[getName()]["path"];

   if(tab == Lua::TAB)
    { n = tab.size();
      for(int i = 1; i <= n; i++)
       { str = (std::string)tab[i];
         resManager->registerSource(new FileSystem(str.c_str()), RES_LOAD);
       }
    }

   tab = lua[getName()]["storage"];
   if(tab == Lua::TAB)
    { n = tab.size();
      for(int i = 1; i <= n; i++)
       { str = (std::string)tab[i];
         resManager->registerSource(new ZipFileSystem(str.c_str()));
       }
    }
   tab = lua[getName()]["save"];
   if(tab == Lua::STR)
    { str = (std::string)tab;
      resManager->registerSource(new FileSystem(str.c_str()), RES_SAVE);
    }
   tab = lua[getName()]["data"];
   if(tab == Lua::TAB)
    { n = tab.size();
      for(int i = 1; i <= n; i++)
       { str = (std::string)tab[i];
         s_data.push_back(str);
       }
    }

   resManager->registerDecoder(new ProgramDecoder(resManager, "sh"));
   resManager->registerDecoder(new MaterialDecoder(resManager, "mat"));
   resManager->registerDecoder(new MeshDecoder(resManager, "ms"));
   //resManager->registerDecoder(new ImageDecoder(resManager, "png"));

   LOG_INFO("%s: first init.", getName());
   return true;
 }

int Factory :: init_update(double tm)
 { Data *data;

   lvm.reg("Load", res_load);
   lvm.reg("Load_data", res_load_data);
   lvm.reg("Unit", create_unit);
   lvm.reg("Message", send_message);

   for(auto &it : s_data)
    { data = resManager->getResource(it);
      if(data)
       { lvm.load_data(data);
         delete data;
       }
    }
   s_data.clear();

   set_var();
   do_update = (MUpdate) &Factory::run_update;
   LOG_INFO("%s: second init.", getName());
   return 1;
 }

int Factory :: run_update(double tm)
 { if(msg_processing())
    Manager :: sendMessage(MSG_DEACTIVE, nullptr, getNumQueue());
   return 1;
 }

int Factory :: clear_update(double tm)
 { delComp();
   entity.erase(std::remove_if(entity.begin(), entity.end(),
                      [](Unit* un)->bool {return un->remove();}),
                      entity.end());
   if((entity.size() > 0)||(resManager->cleaning())) return 1;
   pool[vRES_MANAGER].refCount --;
   del_var();
   LOG_INFO("%s: clear.", getName());
   do_update = &Module::empty_update;
   return 0;
 }

// --------------------------------------------------------------------------------

int Factory :: res_load(luavm vm)
 { Lua::State lua(vm);
   Object *obj = nullptr;
   Lua::Var tmp = lua.sig();

   if(tmp == Lua::STR)
    { std::string str = tmp;
      obj = resManager->getObject(str);
      if(obj) return lua.ret(obj);
    }
   return 0;
 }

int Factory :: res_load_data(luavm vm)
 { Lua::State lua(vm);
   Data *data = 0;
   Lua::Var tmp = lua.sig();

   if(tmp == Lua::STR)
    { std::string str = tmp;
      data = resManager->getResource(str);
      if(data) return lua.ret(data);
    }
   return 0;
 }

int Factory :: send_message(luavm vm)
 { Lua::State lua(vm);
   Lua::Var tmp = lua.sig();
   int msg = tmp;
   int prm = ++tmp;
   Object *obj = nullptr;

   if(tmp.length() > 1)
    { ++tmp;
      if(tmp == Lua::LDATA) obj = tmp;
      else
       LOG_WARNING("Factory: Function Message - the wrong type of the third parameter");
    }
   return lua.ret(Manager::sendMessage(msg, obj, prm));
   return 0;
 }

int Factory :: create_unit(luavm vm)
 { Lua::State lua(vm);
   Lua::Var tab = lua.sig();
   std::string str, mstr;
   Unit *unit;
   Object *obj;
   int pt, n = tab.size();
   unsigned int mid;

   if(tab == Lua::TAB)
    { str = (std::string)tab["name"];
      if(!str.empty())
       { unit = new Unit(str.c_str());
         for(int i = 1; i <= n; i++)
          { Lua::Var tmp = tab[i];
            str = (std::string)tmp["comp"];
            pt = tmp["point"];
            mstr = (std::string)tmp["module"];
            obj = Meta::any_cast<Object*>(Object::invoke(str.c_str(), "create", {&tmp, (unsigned int)pt}));
            mid = Module::getModuleID(mstr.c_str());
            ((Component*)obj)->setModuleId(mid);
            unit->addComp((Component*)obj);
          }
         return lua.ret(unit);
       }
    }
   return 0;
 }
