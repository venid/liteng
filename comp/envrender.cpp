
#include "envrender.h"
#include "unit.h"
#include "log.h"
#include "typevar.h"
#include "luastate.h"
#include "shader.h"
#include "camera.h"
#include <GL/gl.h>

using namespace Render;

META_METHODS(EnvDraw,
 METHOD(create, EnvDraw::Create))
META_PROPERTY(EnvDraw)
META_OBJECT(EnvDraw, Render::EnvDraw, &Component::Instance)

int EnvDraw :: public_tab[] = {vCAMERA, 0};
int EnvDraw :: privat_tab[] = {0};

// --------------------------------------------------------------------------

EnvDraw :: EnvDraw(Lua::Var& tab, unsigned int pt) : Component(pt)
 { Object *obj = tab["program"];

   if(obj) m_shader = (Shader*)obj;
   else
    { m_shader = nullptr;
      LOG_WARNING("Render::EnvDraw: Not detected shader program.");
    }

   public_var = EnvDraw::public_tab;
   privat_var = EnvDraw::privat_tab;
   m_update = (CUpdate)&EnvDraw::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

EnvDraw :: ~EnvDraw()
 { if(m_shader) m_shader->release(); }

void EnvDraw :: linkVar(int def, void* data)
 { switch(def)
    { case vCAMERA:
       mpp_camera = (Camera::Render**)data;
       break;
    }
 }

bool EnvDraw :: init()
 { if(m_shader) m_shader->init();
   return true;
 }

void EnvDraw :: clear()
 { if(m_shader) m_shader->clear(); }

void EnvDraw :: doUpdate()
 { 

 }
