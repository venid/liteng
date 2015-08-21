
#include "version.h"
#include "log.h"

#include "object.h"

Version VERSION;

int main(int argc, char *argv[])
 { if(!Log::Init(Log::Critical | Log::Error | Log::Warning | Log::Debug | Log::Info,
                    "liteng", "----- Little engine -----", VERSION.get())) return 1;

   Object *obj_1, *obj_2;

   obj_1 = new Object("test", Object::genID());

   LOG_INFO("Class %s, name %s", obj_1->getClassName(), obj_1->getName());

   if(obj_1->isClass(Object::Instance))
    obj_2 = obj_1->retain();

   if(!obj_1->remove()) LOG_INFO("obj_1 no remove, refcount = %i", obj_1->getRefCount());

   obj_2->release();
   obj_1->remove();

   Log::Clear();
   return 0;
 }
