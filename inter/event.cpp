
#include "event.h"
#include "log.h"
#include "unit.h"
#include "typevar.h"
#include "data.h"
//#include "manager.h"

using namespace Event;

META_METHODS(Key,
 METHOD(create, Key::Create))
META_PROPERTY(Key)
META_OBJECT(Key, Event::Key, &Component::Instance)

META_METHODS(Mouse,
 METHOD(create, Mouse::Create))
META_PROPERTY(Mouse)
META_OBJECT(Mouse, Event::Mouse, &Component::Instance)

int Key :: public_tab[] = {vLVM, 0};

int Mouse :: public_tab[] = {vLVM, 0};

/// --------------------------------------------------------------------------------------



Key :: Key(Lua::Var& tab, unsigned int pt) : Component(pt)
 { Lua::Var tmp = tab["key"];
   if(tmp == Lua::NUM)
    { m_key = tmp;
      m_update = (CUpdate)&Key::doUpdate;
    }
   else LOG_WARNING("Event::Key: Not detected key.");

   tmp = tab["fun"];
   if(tmp == Lua::LDATA)
    m_fun = tmp;
   else
    { m_fun = nullptr;
      LOG_WARNING("Event::Key: Not detected fun.");
    }

   lvm = nullptr;
   m_triger = 0;
   public_var = Key::public_tab;
   m_id = 0;
   metaClass = &Instance;
 }

Key :: ~Key()
 { if(m_fun) delete m_fun; }

void Key :: linkVar(int def, void* data)
 { if(def == vLVM)
    lvm = (*(Lua::State**)data);
 }

bool Key :: init()
 { if(lvm && m_fun)
    { Lua::Var fn = lvm->load_data(m_fun);
      Lua::Var tab = (*lvm)["EVENTS"];
      tab.reg(getId(), fn);
      return true;
    }
   return false;
 }

void Key :: doUpdate()
 { Lua::State* vm = lvm;
   Lua::Var fn;

   if(m_triger == 0)
    { if(Keyboard::instance().isKeyState((unsigned char)m_key))
       { m_triger = 1;
         fn = (*vm)["EVENTS"][getId()];
         if(fn == Lua::FUN) fn(1);
         else LOG_WARNING("Event::Key: key - %i function not found.", m_key);
         LOG_DEBUG("Event::Key: push button %i", m_key);
       }
    }
   else
    if(!Keyboard::instance().isKeyState((unsigned char)m_key))
     { m_triger = 0;
       fn = (*vm)["EVENTS"][getId()];
       fn(0);
     }
 }

// ---------------------------------------------------------------------------------------

Mouse :: Mouse(Lua::Var& tab, unsigned int pt) : Component(pt)
 { Lua::Var tmp = tab["event"];
   if(tmp == Lua::NUM)
    { m_event = tmp;
      switch(m_event)
       { case M_MOVE: m_update = (CUpdate)&Mouse::doUpdateMove; break;
         case M_WHEELE: m_update = (CUpdate)&Mouse::doUpdateWheele; break;
         case M_LEFT:
         case M_RIGHT:
         case M_MIDDLE: m_update = (CUpdate)&Mouse::doUpdateButton; break;
       }
    }
   else LOG_WARNING("Event::Mouse: Not detected event.");

   tmp = tab["fun"];
   if(tmp == Lua::LDATA)
    m_fun = tmp;
   else
    { m_fun = nullptr;
      LOG_WARNING("Event::Mouse: Not detected function.");
    }

   m_triger = 0;
   lvm = nullptr;
   public_var = Mouse::public_tab;
   m_id = 0;
   metaClass = &Instance;
 }

Mouse :: ~Mouse()
 { if(m_fun) delete m_fun; }

void Mouse :: linkVar(int def, void* data)
 { if(def == vLVM)
    lvm = (*(Lua::State**)data);
 }

bool Mouse :: init()
 { if(lvm && m_fun)
    { Lua::Var fn = lvm->load_data(m_fun);
      Lua::Var tab = (*lvm)["EVENTS"];
      tab.reg(getId(), fn);
      return true;
    }
   return false;
 }

void Mouse :: doUpdateMove()
 { int x, y;
   Lua::State* vm = lvm;
   Lua::Var fn;

   if(Mousedevice::instance().isState(M_MOVE))
    { Mousedevice::instance().getPosAbsolute(x, y);
      fn = (*vm)["EVENTS"][getId()];
      if(fn == Lua::FUN) fn(x, y);
      else LOG_WARNING("Event::Mouse: Missing or incorrect value function (%i)", getId());
    }
 }

void Mouse :: doUpdateButton()
 { Lua::State* vm = lvm;
   Lua::Var fn;
   int x, y;

   if(m_triger == 0)
    { if(Mousedevice::instance().isState(m_event))
       { Mousedevice::instance().getPosAbsolute(x, y);
         m_triger = 1;
         fn = (*vm)["EVENTS"][getId()];
         if(fn == Lua::FUN) fn(m_event, 1, x, y);
         else LOG_WARNING("Event::Mouse: Missing or incorrect value function (%i)", getId());
       }
    }
   else
    if(!Mousedevice::instance().isState(m_event))
     { Mousedevice::instance().getPosAbsolute(x, y);
       m_triger = 0;
       fn = (*vm)["EVENTS"][getId()];
       if(fn == Lua::FUN) fn(m_event, 0, x, y);
       else LOG_WARNING("Event::Mouse: Missing or incorrect value function (%i)", getId());
     }

 }

void Mouse :: doUpdateWheele()
 { Lua::State* vm = lvm;
   Lua::Var fn;
   int flag;

   if(Mousedevice::instance().isState(M_WHEELE))
    { Mousedevice::instance().getWheel(flag);
     // Mousedevice::instance().getPosAbsolute(x, y);
      fn = (*vm)["EVENTS"][getId()];
      if(fn == Lua::FUN) fn(flag); //, x, y);
      else LOG_WARNING("Event::Mouse: Missing or incorrect value function (%i)", getId());
      //LOG_SPAM("Event::Mouse: wheel event %i", flag);
    }
 }
