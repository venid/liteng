
#include "object.h"
#include "log.h"

META_METHODS(Object)
META_PROPERTY(Object)
META_OBJECT(Object, Object, nullptr)

std::atomic<unsigned int> volumeID(1);

Object :: Object() : atf(ATOMIC_FLAG_INIT)
 { name = nullptr;
   id = 0;
   ref_count = 1;
   metaClass = &Instance;
   LOG_DEBUG("Object: The object created by the default constructor");
 }

Object :: Object(const char* theName, unsigned int theId) : atf(ATOMIC_FLAG_INIT)
 { name = (char*)malloc(strlen(theName)+1);
   strcpy(name, theName);
   id = theId;
   ref_count = 1;
   metaClass = &Instance;
   LOG_DEBUG("Object: The object is created with id %i", theId);
 }

Object :: ~Object()
 { free(name); }

void Object :: setName(const char* newName)
 { char* tmp = (char*)malloc(strlen(newName)+1);
   strcpy(tmp, newName);
   free(name);
   name = tmp;
 }

Meta::Any Object :: invoke(const char *Name, std::initializer_list<Meta::Any> args)
 { int index = metaClass->indexOfMethod(Name);
   if (index == -1) return Meta::Any();
   Meta::Method m = metaClass->method(index);
   return m.invoke(this, args.size(), args.begin());
 }

Meta::Any Object :: invoke(const char *className,
                           const char *methodName, std::initializer_list<Meta::Any> args)
 { Meta::Base* cl = Meta::Base::getClass(className);
   if(!cl) return Meta::Any();
   int index = cl->indexOfMethod(methodName);
   if (index == -1) return Meta::Any();
   Meta::Method mt = cl->method(index);
   return mt.invoke(nullptr, args.size(), args.begin());
 }

Object* Object :: retain()
 { while(atf.test_and_set()){}
   ref_count ++;
   LOG_DEBUG("%s: ++ reference count id - %i (%i)", getClassName(), getId(), ref_count);
   atf.clear();
   return this;
 }

bool Object :: release()
 { bool tmp = false;

   while(atf.test_and_set()){}
   ref_count --;
   LOG_DEBUG("%s: -- reference count id - %i (%i)", getClassName(), getId(), ref_count);
   if(ref_count <= 0) tmp = true;
   atf.clear();
   if(tmp)
    { LOG_DEBUG("%s: Release destructor id - %i", getClassName(), getId());
      delete this;
    }
   return tmp;
 }

bool Object :: remove()
 { bool tmp = false;

   while(atf.test_and_set()){}
   if(ref_count < 2) tmp = true;
   atf.clear();
   if(tmp)
    { LOG_DEBUG("%s: Remove destructor id - %i", getClassName(), getId());
      delete this;
    }
   return tmp;
 }

unsigned int Object :: genID()
 { return volumeID ++; }
