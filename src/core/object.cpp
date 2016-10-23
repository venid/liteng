
#include "object.h"
#include "log.h"

META_METHODS(Object)
META_PROPERTY(Object)
META_OBJECT(Object, Object, nullptr)

std::atomic<unsigned int> volumeID(1);

Object :: Object(unsigned int theId) : atf(ATOMIC_FLAG_INIT), id(theId), ref_count(1), metaClass(&Instance)
 { name = nullptr;
   LOG_DEBUG("Object: The object created by the default constructor");
 }

Object :: Object(const char* theName, unsigned int theId) : atf(ATOMIC_FLAG_INIT),
                                                            id(theId),
                                                            ref_count(1),
                                                            metaClass(&Instance)
 { name = (char*)malloc(strlen(theName)+1);
   strcpy(name, theName);
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

bool Object :: release()
 { int count = ref_count --;

   if(count <= 0)
    { LOG_DEBUG("%s: Release destructor id - %i", getClassName(), getId());
      delete this;
      return true;
    }
   return false;
 }

bool Object :: remove()
 { if(getRefCount() < 2)
    { LOG_DEBUG("%s: Remove destructor id - %i", getClassName(), getId());
      delete this;
      return true;
    }
   return false;
 }

unsigned int Object :: genID()
 { return volumeID ++; }
