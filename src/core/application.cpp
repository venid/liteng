
#include "application.h"
#include <algorithm>
#include <string>
#include "log.h"
#include "luastate.h"
#include "object.h"
#include "manager.h"
#include "module.h"
#include "timer.h"

#include "oglrender.h"

Application *app;
int main_thread(luavm vm);
int new_thread(luavm vm);
int create_module(luavm vm);

bool Application::init(const char *configFile)
 { Lua::State lua;

   app = new Application();
   if(!app) return false;

   lua.reg("main", main_thread);
   lua.reg("thread", new_thread);
   lua.reg("module", create_module);
   lua.load_file(configFile);

   LOG_INFO("APP: init complete. (%s)", configFile);
   return true;
 }

void Application :: start()
 { m_timer.getPeriodTime();
   m_status.store(true, std::memory_order_relaxed);
 }


void Application::run()
 { LOG_SPAM("App: run.");
   app->Thread::run();
   LOG_SPAM("App: stop.");
   app->clear();
 }

// ----------------------------------------------------------------

int create_module(luavm vm)
 { Lua::State lua(vm);
   Lua::Var vr = lua.sig();
   Meta::Base* cl;
   std::string strName;

   if(vr == Lua::STR)
    { std::string strClass = vr;
      cl = Meta::Base::getClass(strClass.c_str());
      if(vr.length() > 1)
       { ++ vr;
         strName = std::string(vr);
       }
      else strName = strClass;
      if(cl)
       { Meta::Method mt = cl->method(cl->indexOfMethod("create"));
         lua.ret(Meta::any_cast<Object*>(mt.invoke(nullptr, {strName.c_str(), &lua})));
         return 1;
       }
      LOG_WARNING("App: Unknown module \"%s\"", strClass.c_str());
      return 0;
    }
   LOG_WARNING("App: Undeclared module.");
   return 0;
 }

int new_thread(luavm vm)
 { Object* md;
   Lua::State lua(vm);
   Lua::Var vr = lua.sig();
   int sz = vr.length();
   unsigned int id = Manager::addThread(new Thread());

   for(int i = 1; i <= sz; i++)
    { if(vr.type() == Lua::LDATA)
       { md = vr;
         if(md->isSuperClass(Module::Instance))
          { ((Module*)md)->setThreadID(id);
            Manager::addModule((Module*)md);
          }
       }
      ++ vr;
    }
   return 0;
 }

int main_thread(luavm vm)
 { Object* md;
   Lua::State lua(vm);
   Lua::Var vr = lua.sig();
   int sz = vr.length();
   unsigned int id = Manager::addThread(app);

   for(int i = 1; i <= sz; i++)
    { if(vr.type() == Lua::LDATA)
       { md = vr;
         if(md->isSuperClass(Module::Instance))
          { ((Module*)md)->setThreadID(id);
            ((Module*)md)->addSystem(new Systems::Render()); //<--------------------------
            Manager::addModule((Module*)md);
          }
       }
      ++ vr;
    }
   return 0;
 }
