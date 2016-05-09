#include "unit.h"
#include "component.h"
#include "log.h"
#include "typevar.h"
#include <algorithm>

META_METHODS(Unit)
META_PROPERTY(Unit)
META_OBJECT(Unit, Unit, &Object::Instance)


Unit :: Unit(const char *theName) : Object(theName, Object::genID())
 { metaClass  = &Instance;
   LOG_DEBUG("Unit: Create \"%s\".", theName);
 }

Unit :: ~Unit()
 { for(auto &it : pool)
    VarTable[it.first].second(it.second);
   LOG_DEBUG("Unit: Delete \"%s\".", name);
 }

void Unit :: addComp(Component* cp)
 { int i = 0, def;
   std::map<int, void*>::iterator it;
   void* dat;

   while(cp->privat_var[i])
    { def = cp->privat_var[i];
      it = pool.find(def);
      if(it == pool.end())
       { dat = VarTable[def].first();
         pool.emplace(def, dat);
       }
      else dat = it->second;
      cp->linkVar(def, dat);
      i++;
    }
   cp->setName(getName());
   cp->setOwner(this);
   comp.push_back(cp);
 }

bool Unit :: remove()
 { bool result = false;

   lock();
   comp.erase(std::remove_if(comp.begin(), comp.end(),
             [](Component* cp)->bool {return cp->remove();}),
             comp.end());
   if((comp.size() == 0) && (ref_count <= 1)) result = true;
   unlock();
   if(result) delete this;
   return result;
 }

