#include "control.h"
#include "object.h"
#include "defs.h"
#include "manager.h"
#include "log.h"
#include "input.h"
#include "typevar.h"
#include "component.h"

META_METHODS(Control,
 METHOD(create, Control::Create))
META_PROPERTY(Control)
META_OBJECT(Control, Control, &Module::Instance)

unsigned int Control :: messages_list[] = {MSG_FINISH, MSG_ADD_UNIT, MSG_ADD_SCRIPT, 0};

// ----------------------------------------------------------------------------------

Control :: Control(const char* Name) : Module(Name), lvm()
 { tabMessages = messages_list;
   do_update = (MUpdate)&Control::init_update;
   metaClass = &Instance;
 }

Control :: ~Control()
 { }

bool Control :: init(Lua::State &lua)
 { LOG_INFO("%s: first init.", getName());
   return true;
 }

void Control :: set_var()
 { pool.emplace(vLVM, Link{1, setPointer(&lvm)}); }

bool Control :: msg_processing()
 { int tMsg, param;
   Object* pobj;
   Data* data;

   while(Manager::getMessage(getNumQueue(), tMsg, pobj, param))
    { switch(tMsg)
       { case MSG_ADD_SCRIPT:
          if(pobj)
           { data = pobj->stash;
             lvm.load_data(data);
             pobj->release();
           }
          break;
         case MSG_ADD_UNIT:
          addComp((Unit*)pobj);
          pobj->release();
          break;
         case MSG_FINISH:
          do_update = (MUpdate)&Control::clear_update;
          return false;
       }
    }
   return true;
 }

int Control :: init_update(double tm)
 { set_var();
   lvm.new_tab("EVENTS");
   lvm.reg("Messages", send_message);
   Component::RegMeta(lvm);

   m_dpy = XOpenDisplay( NULL );
   Keyboard::instance().setDisplay(m_dpy);
   Mousedevice::instance().setDisplay(m_dpy);

   Manager :: sendMessage(MSG_ACTIVE, nullptr, getNumQueue());

   LOG_INFO("%s: second init.", getName());
   do_update = (MUpdate)&Control::run_update;
   return 1;
 }

int Control :: clear_update(double tm)
 { delComp();
   pool[vLVM].refCount --;
   del_var();

   Keyboard::instance().setDisplay(nullptr);
   Mousedevice::instance().setDisplay(nullptr);
   XCloseDisplay(m_dpy);

   LOG_INFO("%s: clear.", getName());
   do_update = &Module::empty_update;
   return 0;
 }

int Control :: run_update(double tm)
 { Keyboard::instance().update();
   Mousedevice::instance().update();

   if(msg_processing())
    { for(auto &it : components)
       it.second->update();
    }
   return 1;
 }

int Control :: send_message(luavm vm)
 { Lua::State lua(vm);
   Lua::Var tmp = lua.sig();
   int msg = tmp;
   int prm = ++tmp;
   Object *obj = nullptr;

   if(tmp.length() > 1)
    { ++tmp;
      if(tmp == Lua::LDATA) obj = tmp;
      else
       LOG_WARNING("Control: Function Message - the wrong type of the third parameter");
    }
   return lua.ret(Manager::sendMessage(msg, obj, prm));
 }
