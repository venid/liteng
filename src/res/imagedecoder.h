
#pragma once
#include "resource.h"
#include <string>

class Object;

class ImageDecoder : public ResDecoder
 { private:

   public:
    ImageDecoder(ResManager *theManager, const char* ext);

    Object* decode(const std::string &theName); //возращает указатель на класс Texture
 };

