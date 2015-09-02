
#include "matdecoder.h"
#include "object.h"
#include "data.h"
#include "log.h"
#include "resmanager.h"
#include "material.h"
#include "texture.h"
#include <glm/glm.hpp>

MaterialDecoder :: MaterialDecoder(ResManager *theManager, const char* ext) : ResDecoder(theManager, ext)
 { resMgr->lvm.reg("build_material", buildMat);
 
 }

Object* MaterialDecoder :: decode(const std::string &theName)
 { Object *mat;
   Lua::Var vr;
   Data* dat = resMgr->getResource(theName);
   
   if(dat == nullptr) 
    { LOG_WARNING("MaterialDecoder: Not reading material file \"%s\".", theName.c_str());
      return nullptr;
    }
   
   vr = resMgr->lvm.load_data(dat);
   if(vr == Lua::LDATA) 
    { mat = vr;
      if(mat->isClass(Material::Instance))
       { mat->setName(theName.c_str());
         LOG_INFO("MaterialDecoder: Load material \"%s\" complet.", theName.c_str());
         delete dat;
         return mat;
       }
      else LOG_WARNING("MaterialDecoder: Failed to build material or not the type \"%s\".", theName.c_str());
    }
   delete dat;
   return nullptr;
 }

/// -----------------------------------------------------------------------------------

int MaterialDecoder :: buildMat(luavm vm)
 { Lua::State lua(vm);
   Lua::Var vr = lua.sig();
   Lua::Var tab;
   Material *mat = nullptr;
   glm::vec4 tmp(0.f);
   Object *text;

   mat = new Material("");
   tab = vr["diffuse"];
   if(tab == Lua::TAB)
    { tmp.x = tab[1]; tmp.y = tab[2]; tmp.z = tab[3]; tmp.w = tab[4];
      mat->setDiffuse(tmp);
    }

   tab = vr["ambient"];
   if(tab == Lua::TAB)
    { tmp.x = tab[1]; tmp.y = tab[2]; tmp.z = tab[3]; tmp.w = tab[4];
      mat->setAmbient(tmp);
    }

   tab = vr["specular"];
   if(tab == Lua::TAB)
    { tmp.x = tab[1]; tmp.y = tab[2]; tmp.z = tab[3]; tmp.w = tab[4];
      mat->setSpecular(tmp);
    }

   tab = vr["emission"];
   if(tab == Lua::TAB)
    { tmp.x = tab[1]; tmp.y = tab[2]; tmp.z = tab[3]; tmp.w = tab[4];
      mat->setEmission(tmp);
    }

   tab = vr["shininess"];
   if(tab == Lua::NUM)
    { tmp.x = tab;
      mat->setShininess(tmp.x);
    }

   tab = vr["map1"];
   if(tab == Lua::LDATA)
    { text = tab;
      mat->setMap(0, (Texture*)text);
    }

   tab = vr["map2"];
   if(tab == Lua::LDATA)
    { text = tab;
      mat->setMap(1, (Texture*)text);
    }

   tab = vr["map3"];
   if(tab == Lua::LDATA)
    { text = tab;
      mat->setMap(2, (Texture*)text);
    }

   tab = vr["map4"];
   if(tab == Lua::LDATA)
    { text = tab;
      mat->setMap(3, (Texture*)text);
    }

   lua.ret((Object*)mat);
   return 1;
 }
