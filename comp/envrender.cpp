
#include "envrender.h"
#include "unit.h"
#include "log.h"
#include "typevar.h"
#include "luastate.h"
#include "shader.h"
#include "camera.h"
#include <GL/gl.h>

#include "mesh.h"
#include <glm/gtc/random.hpp>

using namespace Render;

META_METHODS(EnvDraw,
 METHOD(create, EnvDraw::Create))
META_PROPERTY(EnvDraw)
META_OBJECT(EnvDraw, Render::EnvDraw, &Component::Instance)

int EnvDraw :: public_tab[] = {vCAMERA, 0};
int EnvDraw :: privat_tab[] = {0};

// --------------------------------------------------------------------------
Mesh stars("stars");

void initStars();
// -------------------------------------------------------------------------

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

   initStars();
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
   stars.init();
   return true;
 }

void EnvDraw :: clear()
 { if(m_shader) m_shader->clear();
   stars.clear();
 }

void EnvDraw :: doUpdate()
 { glm::mat4 pm = (*mpp_camera)->getProject();
   glm::mat4 vm = (*mpp_camera)->getView();

   vm[3][0] = 0.f;
   vm[3][1] = 0.f;
   vm[3][2] = 0.f;
   pm *= vm;

   glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glDepthMask(GL_FALSE);
   glDepthFunc(GL_LEQUAL);
   glDepthRange(1.f, 1.f);

   glPointSize(3.f);

   m_shader->bind();
   m_shader->setUniformMat("transform.PVM", pm);
   stars.render();

   glDepthRange(0.f, 1.f);
   glPopAttrib();

 }

// -------------------------------------------------------------------

void initStars()
 { Vertex *vert;

   vert = new Vertex[100];

   for(int i = 0; i < 100; i++)
    { vert[i].pos = glm::ballRand(1.0f);
      vert[i].color = glm::vec4(glm::ballRand(1.0f), 1.f);
    }
   stars.setVertex(vert, 100, GL_POINTS);
 }
