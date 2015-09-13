
#pragma once

#include "resource.h"
#include "luastate.h"
#include <string>

class Object;

class MeshDecoder : public ResDecoder
{ private:
   static int buildBox(luavm vm);
  public:
   MeshDecoder(ResManager *theManager, const char* ext);

   Object* decode(const std::string &theName);
};
