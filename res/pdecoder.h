
#pragma once

#include "resource.h"
#include <string>

class Object;

class ProgramDecoder : public ResDecoder
{ public:
   ProgramDecoder(ResManager *theManager, const char* ext) : ResDecoder(theManager, ext)
    {}

   Object* decode(const std::string &theName);
};
