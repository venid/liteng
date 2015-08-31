#include "pdecoder.h"
#include "object.h"
#include "data.h"
#include "log.h"
#include "shader.h"
#include "resmanager.h"

Object* ProgramDecoder :: decode(const std::string &theName)
 { Shader *shader = nullptr;
   Data *vdt, *fdt;
   std::string tmp = theName.substr(0, theName.find_last_of("."));
   std::string str = tmp + ".vsh";

   vdt = resMgr->getResource(str);
   str = tmp + ".fsh";
   fdt = resMgr->getResource(str);
   if(vdt && fdt)
    { shader = new Shader(tmp.c_str());
      shader->vsh = vdt;
      shader->fsh = fdt;
      LOG_INFO("ProgramDecoder: Load shader \"%s\" complet.", tmp.c_str());
      return shader;
    }
   else LOG_WARNING("ProgramDecoder: Not reading shader program \"%s\".", tmp.c_str());
    return nullptr;
 }

