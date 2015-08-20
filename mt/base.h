
#pragma once
 
#include <cstring> 
#include <iostream>

#include "method.h"
#include "any.h"

namespace Meta
{ 

class Base;

struct ApiTable
 { const char* name;
	  const Base *super;
	  const MethodTable *methods;
  	const PropertyTable *props;
	  int methodCount;
	  int propCount;
 };

class Base
 { protected:
    const ApiTable  *table;

    Base        *next;
    static Base *root;
   
  public:
   Base(const ApiTable *theTable);
   ~Base() { }

   const char* getName() const  { return table->name; }
   const Base* getSuper() const { return table->super; }

   int indexOfMethod(const char *) const;
   Method method(int index) const;
   int methodCount() const;
  	int methodOffset() const;

  	int indexOfProperty(const char *name) const;
  	Property property(int) const;
  	int propertyCount() const;
  	int propertyOffset() const;

   bool isSuperClass(const Base& theClass) const
     { const Base* clsPtr = this;

       while ( clsPtr != nullptr)
        { if(*clsPtr == theClass) return true;
          clsPtr = clsPtr ->  getSuper();
        }
       return false;
     }

   bool operator == (const Base& theClass) const  
    { return this == &theClass; }

   static Base* getClass(const char* theName);
   static Method getMethod(const char* theClass, const char* theMethod);
   static Property getProperty(const char* theClass, const char* theProperty);

   static void print()
    { Base *tmp = root;
      while(tmp)
       { printf("%s \n", tmp->getName());
         tmp = tmp->next;
       }
    }  

 };
 
} 
