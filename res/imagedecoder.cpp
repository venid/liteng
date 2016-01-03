
#include "imagedecoder.h"
#include "log.h"
#include "texture.h"
#include "data.h"
#include "resmanager.h"

ImageDecoder :: ImageDecoder(ResManager *theManager, const char* ext) : ResDecoder(theManager, ext)
 { }

Object* ImageDecoder :: decode(const std::string &theName)
 { Object *texture;
   Data* dat = resMgr->getResource(theName);

   if(dat == nullptr)
    { LOG_WARNING("ImageDecoder: Not reading image file \"%s\".", theName.c_str());
      return nullptr;
    }

   texture = new Texture(theName.c_str());
   if(texture)
    { LOG_INFO("ImageDecoder: Load texture \"%s\" complet.", theName.c_str());
      delete dat;
      return texture;
    }
   else LOG_WARNING("ImageDecoder: Failed to load texture or not the type \"%s\".", theName.c_str());

   delete dat;
   return nullptr;
 }

