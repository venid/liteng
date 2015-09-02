
#pragma once
#include "resource.h"
#include "luastate.h"
#include <string>

class Object;

class MaterialDecoder : public ResDecoder
 { private:
    static int buildMat(luavm vm);
   public:
    MaterialDecoder(ResManager *theManager, const char* ext);

    Object* decode(const std::string &theName);
 };

