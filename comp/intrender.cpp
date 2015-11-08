
#include "intrender.h"
#include "unit.h"
#include "log.h"
#include "typevar.h"
#include "luastate.h"
#include "shader.h"
#include "camera.h"
#include <GL/gl.h>

using namespace Render;

META_METHODS(IntDraw,
 METHOD(create, IntDraw::Create))
META_PROPERTY(IntDraw)
META_OBJECT(IntDraw, Render::IntDraw, &Component::Instance)

int IntDraw :: public_tab[] = {vCAMERA, 0};
int IntDraw :: privat_tab[] = {0};

// --------------------------------------------------------------------------

IntDraw :: IntDraw(Lua::Var& tab, unsigned int pt) : Component(pt)
 { Object *obj = tab["program"];

   if(obj) m_shader = (Shader*)obj;
   else
    { m_shader = nullptr;
      LOG_WARNING("Render::IntDraw: Not detected shader program.");
    }

   public_var = IntDraw::public_tab;
   privat_var = IntDraw::privat_tab;
   m_update = (CUpdate)&IntDraw::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

IntDraw :: ~IntDraw()
 { if(m_shader) m_shader->release(); }

void IntDraw :: linkVar(int def, void* data)
 { switch(def)
    { case vCAMERA:
       mpp_camera = (Camera::Render**)data;
       break;
    }
 }

bool IntDraw :: init()
 { if(m_shader) m_shader->init();
   return true;
 }

void IntDraw :: clear()
 { if(m_shader) m_shader->clear(); }

void IntDraw :: doUpdate()
 { 

 }
