#include "control.h"
#include "object.h"
#include "defs.h"
#include "manager.h"
#include "log.h"
#include "input.h"
#include "typevar.h"
#include "component.h"

META_METHODS(Control,
 METHOD(create, Control::Create),
 METHOD(clear, Control::clear),
 METHOD(add, Control::add),
 METHOD(addScript, Control::addScript))
META_PROPERTY(Control)
META_OBJECT(Control, Control, &Module::Instance)

// ----------------------------------------------------------------------------------

Control :: Control(const char* Name) : Module(Name), lvm()
 { do_update = (MUpdate)&Control::init_update;
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

void Control :: connectMsg()
 { addMsg(MSG_FINISH, this, "clear");
   addMsg(MSG_ADD_UNIT, this, "add");
   addMsg(MSG_ADD_SCRIPT, this, "addScript");
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

int Control :: run_update(double tm)
 { Keyboard::instance().update();
   Mousedevice::instance().update();

   for(auto it = components.upper_bound(0); it != components.end(); it++)
    (*it).second->update();

   return 1;
 }

void Control :: clear(Object* obj, int param)
 { delComp();
   pool[vLVM].refCount --;
   del_var();

   Keyboard::instance().setDisplay(nullptr);
   Mousedevice::instance().setDisplay(nullptr);
   XCloseDisplay(m_dpy);

   LOG_INFO("%s: clear.", getName());
   do_update = &Module::empty_update;
 }

void Control :: add(Object* pobj, int param)
 { addComp((Unit*)pobj);
   pobj->release();
 }

void Control :: addScript(Object* pobj, int param)
 { Data* data;

   if(pobj)
    { data = (Data*)pobj;
      lvm.load_data(data);
      pobj->release();
    }
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
