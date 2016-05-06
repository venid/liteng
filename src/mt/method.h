
#pragma once

#include "any.h"
#include "type.h"

namespace Meta
{

typedef Any(*InvokeMem)(void *, int argc, const Any *);
typedef Any(*ReadMem)(void *);
typedef void(*WriteMem)(void *, const Any&);

struct MethodTable
 { const char* name;
	  InvokeMem invoker;
	  int argc;
	  const TypeTable **types;
 };

class Method
 { public:
	  	Method(const MethodTable *table);

	  	bool valid() const;

	 	 const char* name() const;

	  	Type returnType() const;

	  	int parameterCount() const;
	  	Type parmaeterType(int index) const;

	  	Any invoke(void *obj, int argc, const Any *args) const;
   	Any invoke(void *obj, std::initializer_list<Any> args) const;

   private:
	   const MethodTable *_table;
 };

/// --------------------------------------------------------------

struct PropertyTable
 { const char* name;
			const TypeTable *type;
			ReadMem reader;
			WriteMem writer;
 };

class Property
 { public:
	  	Property(const PropertyTable *table);

	  	bool valid() const;

    const char* name() const;

	  	Type type() const;
	
    Any read(void *obj) const;
    void write(void *obj, const Any& value) const;

   private:
	   const PropertyTable *_table;
 }; 

}
