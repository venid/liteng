
#include "meshdecoder.h"
#include "object.h"
#include "data.h"
#include "log.h"
#include "resmanager.h"
#include "mesh.h"
#include "vertex.h"
#include <glm/glm.hpp>

MeshDecoder :: MeshDecoder(ResManager *theManager, const char* ext) : ResDecoder(theManager, ext)
 { resMgr->lvm.reg("build_box", buildBox); }

Object* MeshDecoder :: decode(const std::string &theName)
 { Object *mesh;
   Lua::Var vr;

   Data* dat = resMgr->getResource(theName);
   if(dat == nullptr)
    { LOG_WARNING("MeshDecoder: Not reading mesh file \"%s\".", theName.c_str());
      return nullptr;
    }

   vr = resMgr->lvm.load_data(dat);
   if(vr == Lua::LDATA)
    { mesh = vr;
      if(mesh->isClass(Mesh::Instance))
       { mesh->setName(theName.c_str());
         LOG_INFO("MeshDecoder: Load material \"%s\" complet.", theName.c_str());
         delete dat;
         return mesh;
       }
      else LOG_WARNING("MeshDecoder: Failed to build or not the type \"%s\".", theName.c_str());
    }
   delete dat;
   return nullptr;
 }

// -------------------------------------------------------------------------------

int MeshDecoder :: buildBox(luavm vm)
 { Lua::State lua(vm);
   Lua::Var vr = lua.sig();
   float x, y, z;
   Object *obj = nullptr;

   if(vr.length() == 3)
    { x = vr; ++vr;
      y = vr; ++vr;
      z = vr;
      obj = Mesh::makeBox(x, y, z);
    }
   lua.ret(obj);
   return 1;
 }

//int MeshDecoder :: buildGeoSphere(luavm vm)
 //{ Lua::State lua(vm);
   //Lua::Var vr = lua.sig();
   //float r;
   //int i, flag = 0;
   //Object *obj = nullptr;

   //if(vr.length() > 1)
    //{ r = vr; ++vr; 
      //i = vr;
      //if(vr.length() > 1)
       //{ ++ vr;
         //flag = vr;
       //}
      //obj = Mesh::makeSphere(r, i, flag);
    //}
   //return lua.ret(obj);
 //}

//int MeshDecoder :: buildMesh(luavm vm)
 //{ Lua::State lua(vm);
   //Lua::Var sig = lua.sig();
   //Lua::Var tmp;
   //Object *obj = nullptr;

   //int n = sig.size();
   //glm::vec3 *vert = new glm::vec3[n];
   //for(int i = 0; i < n; i++)
    //{ tmp = sig[i+1];
      //vert[i].x = tmp[1];
      //vert[i].y = tmp[2];
      //vert[i].z = tmp[3];
    //}

   //++ sig;
   //n = sig.size();
   //glm::vec2 *text = new glm::vec2[n];
   //for(int i = 0; i < n; i++)
    //{ tmp = sig[i+1];
      //text[i].x = tmp[1];
      //text[i].y = tmp[2];
    //}

   //++ sig;
   //n = sig.size();

   //obj = Mesh::makeBox(0.1f, 0.2f, 0.3f);

   //return lua.ret(obj);
 //}

