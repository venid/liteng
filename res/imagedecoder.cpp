
#include "imagedecoder.h"
#include "log.h"
#include "texture.h"
#include "data.h"
#include "resmanager.h"
#include "image.h"

ImageDecoder :: ImageDecoder(ResManager *theManager, const char* ext) : ResDecoder(theManager, ext)
 { }

Object* ImageDecoder :: decode(const std::string &theName)
 { Texture* texture;
   Data* dat = resMgr->getResource(theName);
   Image* img;

   if(dat == nullptr)
    { LOG_WARNING("ImageDecoder: Not reading image file \"%s\".", theName.c_str());
      return nullptr;
    }

   texture = new Texture(theName.c_str());
   if(texture)
    { img = new Image();
      if(img && img->loadPNG(dat))
       { texture->addImage(img);
         LOG_INFO("ImageDecoder: Load texture \"%s\" complet.", theName.c_str());
       }
      delete dat;
      return texture;
    }
   else LOG_WARNING("ImageDecoder: Failed to load texture or not the type \"%s\".", theName.c_str());

   delete dat;
   return nullptr;
 }

