/*
 14.04.2014
*/

#pragma once

#include <atomic>
#include "any.h"
#include "mt/meta.h"

#include <stdio.h>

class Object
 {
  protected:
   char *name;
   std::atomic_uint id;
   std::atomic_int  ref_count;
   std::atomic_flag atf;
   Meta::Base *metaClass;

  public:
   Any stash;

   Object();
   Object(const char* theName, unsigned int theId = 0);
   virtual ~Object();

   const char* getName() const
    { return name; }
   void setName(const char* newName);

   unsigned int getId() const
    { return id.load(std::memory_order_relaxed); }
   void setId(unsigned int newId)
    { id.store(newId, std::memory_order_relaxed); }

   inline Meta::Base* meta() const { return metaClass; }

   const char* getClassName() const
    { return metaClass->getName(); }

   bool isSuperClass(const Meta::Base& theClass) const
    { return metaClass->isSuperClass(theClass); }

   bool isClass(const Meta::Base& theClass) const
    { return metaClass == &theClass ? true : false; }

   Meta::Any invoke(const char *Name,
                    std::initializer_list<Meta::Any> args);

   static Meta::Any invoke(const char *className,
                           const char *methodName,
                           std::initializer_list<Meta::Any> args);

   bool isMethod(const char *Name) const
    { if(metaClass->indexOfMethod(Name) == -1) return false;
      else return true;
    }
   bool isProperty(const char *Name) const
     { if(metaClass->indexOfProperty(Name) == -1) return false;
      else return true;
    }

   void lock() { while(atf.test_and_set()){} }
   void unlock() { atf.clear(); }

   inline Object* retain()
    { ref_count ++;
      return this;
    }
   bool release();
   bool remove();

   inline int getRefCount() const
    { return ref_count.load(std::memory_order_relaxed); }

   static Meta::Base Instance;
   static unsigned int genID();
 };

