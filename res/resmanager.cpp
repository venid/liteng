
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include "resmanager.h"
#include "log.h"
#include "data.h"
#include "object.h"

ResManager :: ResManager(luavm vm) : lvm(vm)
 { saveRes = nullptr; }

ResManager :: ~ResManager()
 { std::for_each(sources.begin(), sources.end(),
                   [](Resource* res){delete res;});

   std::for_each(decoders.begin(), decoders.end(),
                   [](ResDecoder* dec){delete dec;});

   sources.clear();
   decoders.clear();
 }

int ResManager :: registerSource(Resource *system, bool resType)
 { sources.push_back(system);
   if(resType == RES_SAVE) saveRes = system;
   return 1;
 }

int ResManager :: registerDecoder(ResDecoder * decoder)
 { decoders.push_back(decoder);
   return 1;
 }

Data* ResManager :: getResource(const std::string& theName)
 { Data* data;

   for(auto it = sources.begin(); it != sources.end(); ++it)
    if((data = (*it) -> getFile(theName)) != nullptr) return data;

   LOG_WARNING("ResManager: \"%s\" resource not found.", theName.c_str());
   return nullptr;
 }

Object* ResManager :: getObject(const std::string& theName)
 { Object *object = nullptr;
   bool read = false;

   for(auto &it : storage)
    if(strcmp(it->getName(), theName.c_str()) == 0)
     return it->retain();

   for(auto &it : decoders)
    if (it->checkExtension(theName))
     { read = true;
       object = it->decode(theName);
       if(object)
        { storage.push_back(object);
          return object->retain();
        }
     }

   if(!read) LOG_WARNING("ResManager: unknown extension \"%s\"", theName.c_str());

   return nullptr;
 }

void ResManager :: regObject(Object* obj)
 { const char* Name = obj->getName();

   for(auto &it : storage)
    if(strcmp(it->getName(), Name) == 0)
     { LOG_WARNING("ResManager: %s, is not possible to register the resource.(name clash)", Name);
       return;
     }
   storage.push_back(obj);
 }

bool ResManager :: saveResource(const std::string& theName, Data *data)
 { if(saveRes && saveRes->saveFile(theName, data))
    { LOG_INFO("ResManager: save \"%s\" successfully.", theName.c_str());
      return true;
    }
   LOG_WARNING("ResManager: cannot save \"%s\"", theName.c_str());
   return false;
 }

int ResManager :: cleaning()
 { storage.erase(std::remove_if(storage.begin(), storage.end(),
                                [](Object* obj)->bool {return obj->remove();}),
                                storage.end());
   return storage.size();
 }
