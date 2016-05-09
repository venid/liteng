
#include "module.h"
#include "component.h"
#include "unit.h"
#include "log.h"
#include "typevar.h"
#include "manager.h"
#include "defs.h"
#include <malloc.h>
#include <cstring>
#include <algorithm>

META_METHODS(Module)
META_PROPERTY(Module)
META_OBJECT(Module, Module, &Object::Instance)

std::vector<Module*> Module :: module_tab;


Module :: Module(const char* Name) : Object(Name, Object::genID())
 { action = false;
   thrID = 0;
   do_update = &Module::empty_update;
   module_tab.push_back(this);
   metaClass = &Instance;
   LOG_INFO("Module: Create \"%s\"", Name);
 }

Module :: ~Module()
 { LOG_INFO("Module: Delete \"%s\"", getName()); }

bool Module :: isComp(unsigned int Id)
 { for(auto it : components)
    if(it.second->getId() == Id) return true;
   return false;
 }

void Module :: addMsg(unsigned int msg, Object* obj, const char* theName)
 { Meta::Base* base = obj->meta();
   Meta::Method mt = base->method(base->indexOfMethod(theName));
   std::pair<Object*, Meta::Method> hdr(obj, mt);
   msgHandlers.emplace(msg, hdr);
   Manager::sendMessage(MSG_CONNECT, this, msg);
 }

void Module :: addComp(Unit* un)
 { int i, def;
   unsigned int Id = getId();
   void* dat;
   std::map<int, Link>::iterator vr;

   for(auto &it : un->comp)
    { if(it->getModuleId() != Id) continue;
      if(isComp(it->getId())) continue;
      i = 0;
      while(it->public_var[i])
       { def = it->public_var[i];
         vr = pool.find(def);
         if(vr == pool.end())
          { dat = VarTable[def].first();
            pool.emplace(def, Link{1, dat});
          }
         else
          { dat = vr->second.data;
            vr->second.refCount ++;
          }
         it->linkVar(def, dat);
         i++;
       }
      it->init();
      it->retain();
      components.emplace(it->getPoint(), it);
      it->connect(this);
      LOG_SPAM("%s: Add component id %i", getName(), it->getId());    }
 }

void Module :: delComp()
 { int i;
   std::map<int, Link>::iterator vr;

   for(auto &it : components)
    { i = 0;
      while(it.second->public_var[i])
       { vr = pool.find(it.second->public_var[i]);
         if(vr != pool.end())
          vr->second.refCount --;
         i++;
       }
      it.second->clear();
      it.second->release();
    }
   components.clear();
 }

void Module :: delComp(unsigned int uID)
 { int i;
   std::map<int, Link>::iterator vr;

   for(auto it = components.begin(); it != components.end();)
    { if( it->second->getUnitId() == uID )
       { i = 0;
         while(it->second->public_var[i])
          { vr = pool.find(it->second->public_var[i]);
            if(vr != pool.end())
             vr->second.refCount --;
            i++;
          }
         it->second->clear();
         it->second->release();

         it = components.erase(it);
       }
      else ++it;
    }
 }

void Module :: del_var()
 { for(auto it = pool.begin(), ite = pool.end(); it != ite;)
    { if(it->second.refCount < 1)
       { VarTable[it->first].second(it->second.data);
         it = pool.erase(it);
       }
      else ++it;
    }
 }

unsigned int Module :: getModuleID(const char* Name)
 { for(auto &it : module_tab)
    if(strcmp(Name, it->getName()) == 0) return it->getId();
   return 0;
 }

int Module :: update(double tm)
 { int tMsg, param;
   Object* pobj;
   std::vector<std::pair<Object*, Meta::Method> > tmp;

   while(Manager::getMessage(getNumQueue(), tMsg, pobj, param))
    { auto ret = msgHandlers.equal_range(tMsg);
      if(ret.first != msgHandlers.end())
       { for(auto it = ret.first; it != ret.second; ++it)
          tmp.push_back(it->second);

         for(auto it : tmp)
          it.second.invoke(it.first, {pobj, param});

         tmp.erase(tmp.begin());
         if(pobj) pobj->release();
       }
    }
   return (this->*do_update)(tm);
 }
