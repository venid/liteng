
#include "component.h"
#include "log.h"
#include "unit.h"
#include <typeinfo>

META_METHODS(Component)
META_PROPERTY(Component)
META_OBJECT(Component, Component, &Object::Instance)

int Component :: empty_tab[] = {0};

// ----------------------------------------------------------

Component :: Component(unsigned int point) : Object()
 { id = Object::genID();
   m_point = point;
   m_update = &Component::doUpdate;
   m_id = 0;
   m_im = -1;
   m_unit = nullptr;
   privat_var = empty_tab;
   public_var = empty_tab;
   metaClass  = &Instance;
   LOG_DEBUG("Component: Create id = %i.", getId());
 }

Component :: ~Component()
 { LOG_DEBUG("Component: Delete id = %i.", getId()); }

void Component :: RegMeta(Lua::State& st)
 { Lua::Var tmp = st["Component"];
   if(tmp != Lua::TAB)
    { tmp = st.new_tab("Component", Lua::META);
      tmp.reg("__index", getting);
      tmp.reg("__newindex", setting);
    }
 }

unsigned int Component :: getUnitId()
 { if(m_unit) return m_unit->getId();
   else return 0;
 }

// -----------------------------------------------------------

int Component :: getting(luavm vm)
 { Lua::State lvm(vm);
   Lua::Var sig = lvm.sig();
   Component *cmp = (Component*)(*(Object**)sig);
   ++ sig;
   std::string str = (std::string)sig;
   int im = cmp->metaClass->indexOfMethod(str.c_str());
   if(im != -1)
    { cmp->m_im = im;
      return lvm.ret(calling);
    }
   else
    { im = cmp->metaClass->indexOfProperty(str.c_str());
      if(im != -1)
       { Meta::Property prt = cmp->metaClass->property(im);
         Meta::Type tp = prt.type();
         Meta::Any arg = prt.read(cmp);
         if(tp.id() == typeid(int))
          return lvm.ret(Meta::any_cast<int>(arg));
         if(tp.id() == typeid(float))
           //LOG_SPAM("Component: %s call function getting(%s)", cmp->getName(), str.c_str());
          return lvm.ret(Meta::any_cast<float>(arg));
       }
    }
   //LOG_SPAM("Component: %s call function getting(%s)", cmp->getName(), str.c_str());
   return 0; //lvm.ret(calling);
 }

int Component :: setting(luavm vm)
 { Lua::State lvm(vm);
   Lua::Var sig = lvm.sig();
   Component *cmp = (Component*)(*(Object**)sig);
   ++ sig;
   std::string str = (std::string)sig;
   ++ sig;
   Meta::Property prt = cmp->metaClass->property(cmp->metaClass->indexOfProperty(str.c_str()));
   Meta::Type tp = prt.type();
   Meta::Any arg;
   if(tp.id() == typeid(int))
    { arg.reset((int)sig);
      prt.write(cmp, arg);
      return 0;
    }
   if(tp.id() == typeid(float))
    { arg.reset((float)sig);
      prt.write(cmp, arg);
      return 0;
    }

   return 0;
 }

int Component :: calling(luavm vm)
 { int mcount;
   Lua::State lvm(vm);
   Lua::Var sig = lvm.sig();
   Component *cmp = (Component*)(*(Object**)sig);
   ++ sig;
   if(cmp->m_im != -1)
    { Meta::Method mt = cmp->metaClass->method(cmp->m_im);
      mcount = mt.parameterCount();
      if(mcount > sig.length()) return 0;
      Meta::Any *args = new Meta::Any[mcount];
      for (int i = 0; i < mcount; ++i)
       { Meta::Type tp = mt.parmaeterType(i);
         if(tp.id() == typeid(int))
          { args[i].reset((int)sig);
            ++ sig;
            continue;
          }
         if(tp.id() == typeid(float))
          { args[i].reset((float)sig);
            ++ sig;
            continue;
          }
       }
      Meta::Any res = mt.invoke(cmp, mcount, args);
    }
   return 0;
 }
