
#include "camera.h"
#include "log.h"
#include "typevar.h"
#include "unit.h"
#include "input.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Camera;

META_METHODS(Render,
 METHOD(create, Render::Create))
META_PROPERTY(Render)
META_OBJECT(Render, Camera::Render, &Component::Instance)

META_METHODS(Translate,
 METHOD(create, Translate::Create))
META_PROPERTY(Translate)
META_OBJECT(Translate, Camera::Translate, &Component::Instance)

META_METHODS(Control,
 METHOD(create, Control::Create),
 METHOD(setFlag, Control::setFlag),
 METHOD(setCursor, Control::setCursor),
 METHOD(rotation, Control::rotation),
 METHOD(dirDist, Control::dirDist))
META_PROPERTY(Control)
META_OBJECT(Control, Camera::Control, &Component::Instance)

int Render :: privat_tab[] = {vCAM_TRANSLATE, vCAM_POS, 0};
int Render :: public_tab[] = {vCAMERA, vWIN_RECT, 0};

int Translate :: privat_tab[] = {vCAM_TRANSLATE, vCAM_ORBIT_DIST, vCAM_TURN, vCAM_POS, 0};

int Control :: public_tab[] = {vLVM, 0};
int Control :: privat_tab[] = {vCAM_ORBIT_DIST, vCAM_TURN, 0};

// ------------------------------------------------------------------------------

Render :: Render(Lua::Var& tab, unsigned int pt) : Component(pt)
 { Lua::Var var = tab["zfar"];
   if(var == Lua::NUM) m_zfar = var;
   else
    { LOG_WARNING("Camera::Render: Not detected zfar.");
      m_zfar = 0.1f;
    }
   var = tab["znear"];
   if(var == Lua::NUM) m_znear = var;
   else
    { LOG_WARNING("Camera::Render: Not detected znear.");
      m_znear = 100.f;
    }
   var = tab["fovx"];
   if(var == Lua::NUM) m_fov = glm::radians((float)var);
   else
    { LOG_WARNING("Camera::Render: Not detected fovx.");
      m_fov = glm::radians(50.f);
    }

   privat_var = Render::privat_tab;
   public_var = Render::public_tab;
   m_update = (CUpdate)&Render::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

Render :: ~Render() { }

bool Render :: init()
 { float aspectRation = (float)rect.x/(float)rect.y;
   m_fovY = 1.f / tanf(0.5f * m_fov);
   m_fovX = m_fovY / aspectRation;
   m_proj = glm::perspective(m_fov, aspectRation, m_znear, m_zfar);
   return true;
 }

void Render :: linkVar(int def, void* data)
 { glm::ivec2 *iv2;
   Render** cam;

   switch(def)
    { case vCAM_TRANSLATE:
       mp_transMatrix = (glm::mat4*)data;
       break;
      case vWIN_RECT:
       iv2 = (glm::ivec2*)data;
       rect.x = iv2->x;
       rect.y = iv2->y;
       break;
      case vCAMERA:
       cam = (Render**)data;
       *cam = this;
       break;
      case vCAM_POS:
       mp_pos = (glm::vec3*)data;
       break;
    }
 }


void Render :: doUpdate()
 { m_unit->lock();
   memcpy(&m_view, mp_transMatrix, sizeof(glm::mat4));
   memcpy(&m_pos, mp_pos, sizeof(glm::vec3));
   m_unit->unlock();

   m_pvMatrix = m_proj * m_view;
   buildFrustrum(m_view);
 }

void Render :: buildFrustrum(glm::mat4& mt)
 { glm::vec3 viewDir(mt[0][2], mt[1][2], mt[2][2]);
   glm::vec3 rightDir(mt[0][0], mt[1][0], mt[2][0]);
   glm::vec3 upDir(mt[0][1], mt[1][1], mt[2][1]);

   viewDir *= -1.f;
   glm::vec3 v(viewDir  * m_zfar );
   glm::vec3 hr(rightDir * (m_zfar / m_fovX) );
   glm::vec3 hu(upDir    * (m_zfar / m_fovY) );
   glm::vec3 vv[4];

   vv [0] = m_pos + v + hr + hu;
   vv [1] = m_pos + v - hr + hu;
   vv [2] = m_pos + v - hr - hu;
   vv [3] = m_pos + v + hr - hu;

   m_frustrum.set(m_pos, vv, 4);
   Plane pl = Plane(-viewDir, m_pos + m_zfar * viewDir);
   m_frustrum.addPlane(pl);
   pl = Plane(viewDir, m_pos + m_znear * viewDir);
   m_frustrum.addPlane(pl);
 }

// --------------------------------------------------------------

Translate :: Translate(unsigned int pt) : Component(pt)
 { m_orbitDist = 5.f;

   m_orientation = glm::quat(1.f, 0.f, 0.f, 0.f);
   m_pos = glm::vec3(0.f);
   m_target = glm::vec3(0.f);

   privat_var = Translate::privat_tab;
   m_update = (CUpdate)&Translate::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

Translate :: ~Translate()
 { }

void Translate :: linkVar(int def, void* data)
 { switch(def)
    { case vCAM_POS:
       mp_pos = (glm::vec3*)data;
       break;
      case vCAM_TRANSLATE:
       mp_transMatrix = (glm::mat4*)data;
       break;
      case vCAM_ORBIT_DIST:
       mp_dist = (float*)data;
       break;
      case vCAM_TURN:
       mp_turn = (glm::vec2*)data;
       break;
    }
 }

void Translate :: doUpdate()
 { m_unit->lock();
   float dst = *mp_dist;
   glm::vec2 tmp(mp_turn->x, mp_turn->y);
   *mp_turn = glm::vec2(0.f);
   m_unit->unlock();

   m_orbitDist += dst;
   if((tmp.x != 0.f)||(tmp.y != 0.f)) rotate(tmp.y * 0.005f, tmp.x * 0.005f, 0.f);
   updateTransMatrix();

   m_unit->lock();
   memcpy(mp_transMatrix, &m_matrix, sizeof(glm::mat4));
   memcpy(mp_pos, &m_pos, sizeof(glm::vec3));
   m_unit->unlock();
 }

void Translate :: updateTransMatrix()
 { m_matrix = glm::mat4_cast(m_orientation);

   glm::vec3 zAxis = glm::vec3(m_matrix[0][2], m_matrix[1][2], m_matrix[2][2]);
   m_pos = m_target + zAxis * m_orbitDist;

   m_matrix[3][0] = - glm::dot(glm::vec3(m_matrix[0][0], m_matrix[1][0], m_matrix[2][0]), m_pos);
   m_matrix[3][1] = - glm::dot(glm::vec3(m_matrix[0][1], m_matrix[1][1], m_matrix[2][1]), m_pos);
   m_matrix[3][2] = - glm::dot(zAxis, m_pos);
 }

void Camera::Translate :: rotateOrbit(float headingDegrees, float pitchDegrees)
 { glm::quat rot;

   if (headingDegrees != 0.0f)
    { rot = glm::angleAxis(headingDegrees, glm::vec3(1.f, 0.f, 0.f));
      m_orientation = rot * m_orientation;
    }
   if (pitchDegrees != 0.0f)
    { rot = glm::angleAxis(pitchDegrees, glm::vec3(0.f, 1.f, 0.f));
      m_orientation = m_orientation * rot;
    }
 }

void Translate :: rotate(float headingDegrees, float pitchDegrees, float rollDegrees)
 { pitchDegrees = -pitchDegrees;
   headingDegrees = -headingDegrees;
   rollDegrees = -rollDegrees;

   rotateOrbit(headingDegrees, pitchDegrees);
   m_orientation = glm::normalize(m_orientation);
 }

// ----------------------------------------------------------------------------------------------

Control :: Control(unsigned int pt) : Component(pt)
 { m_flag = 0;
   m_update = (CUpdate)&Control::doUpdate;
   public_var = Control::public_tab;
   privat_var = Control::privat_tab;
   m_id = 0;
   metaClass = &Instance;
 }

Control :: ~Control()
 { }

void Control :: linkVar(int def, void* data)
 { switch(def)
    { case vLVM:
       lvm = (*(Lua::State**)data);
       break;
      case vCAM_ORBIT_DIST:
       mp_dist = (float*)data; *mp_dist = 0.0f;
       break;
      case vCAM_TURN:
       mp_turn = (glm::vec2*)data;
       break;
    }
 }

bool Control :: init()
 { regComp(*lvm);
   return true;
 }

void Control :: doUpdate()
 { //Control* cmp = (Control*)comp;
   //auto key = (unsigned char)cmp->m_event;
   //Lua::State* vm = cmp->lvm;

 }

void Control :: dirDist(float volume)
 { m_unit->lock();
   *mp_dist += volume;
   m_unit->unlock();
 }

void Control :: rotation(int x, int y)
 { if(m_flag == 1)
    { m_unit->lock();
      *mp_turn = glm::vec2((float)(x - m_cursor.x), (float)(y - m_cursor.y));
      m_unit->unlock();
      m_cursor = glm::ivec2(x, y);
    }
 }
