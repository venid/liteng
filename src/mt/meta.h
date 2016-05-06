
#include "base.h"
#include "methoddefs.h"
#include "method.h"
#include "any.h"
#include "type.h"

#define METHOD(n, m) \
 { #n, \
   (Meta::InvokeMem)&Meta::Invoker<decltype(&m)>::template invoke<&m>, \
	  Meta::Invoker<decltype(&m)>::argCount(), \
	  Meta::Invoker<decltype(&m)>::types() \
 }
#define OVERMETHOD(n, m, s) \
 { #n, \
   (Meta::InvokeMem)&Meta::Invoker<s>::template invoke<&m>, \
	  Meta::Invoker<s>::argCount(), \
	  Meta::Invoker<s>::types() \
 }
#define PROP(p, r, w) \
 { #p, \
	  Meta::Reader<decltype(&r), &r>::type(), \
	  &Meta::Reader<decltype(&r), &r>::read, \
	  &Meta::Writer<decltype(&w), &w>::write \
 }

#define META_METHODS(Class, ...) \
 static Meta::MethodTable Class##Method[] { __VA_ARGS__ };

#define META_PROPERTY(Class, ...) \
 static Meta::PropertyTable Class##Prop[] { __VA_ARGS__ };

#define META_OBJECT(Class, Name, Super) \
 static Meta::ApiTable Class##Table {#Name, \
                               Super, \
                               Class##Method, \
                               Class##Prop, \
                               sizeof(Class##Method) / sizeof(Meta::MethodTable), \
                               sizeof(Class##Prop) / sizeof(Meta::PropertyTable)}; \
 Meta::Base Class :: Instance(&Class##Table);
