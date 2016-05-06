
#include "camera.h"
#include "log.h"
#include "typevar.h"
#include "unit.h"
#include "input.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace Camera;

META_METHODS(Render,
 METHOD(create, Render::Create))
META_PROPERTY(Render)
META_OBJECT(Render, Camera::Render, &Component::Instance)

META_METHODS(Flight,
 METHOD(create, Flight::Create))
META_PROPERTY(Flight)
META_OBJECT(Flight, Camera::Flight, &Component::Instance)

META_METHODS(Orbit,
 METHOD(create, Orbit::Create))
META_PROPERTY(Orbit)
META_OBJECT(Orbit, Camera::Orbit, &Component::Instance)

META_METHODS(Translate,
 METHOD(create, Translate::Create))
META_PROPERTY(Translate)
META_OBJECT(Translate, Camera::Translate, &Component::Instance)

META_METHODS(Control,
 METHOD(create, Control::Create),
 METHOD(setFlag, Control::setFlag),
 METHOD(setCursor, Control::setCursor),
 METHOD(rotation, Control::rotation))
META_PROPERTY(Control,
 PROP(distance, Control::getDistance, Control::setDistance),
 PROP(types, Control::getType, Control::setType))
META_OBJECT(Control, Camera::Control, &Component::Instance)

int Render :: privat_tab[] = {vCAM_DIMENSIONS, vCAM_VIEW, 0};
int Render :: public_tab[] = {vCAMERA, vWIN_RECT, 0};

int Flight :: privat_tab[] = {vCAM_TYPE, vCAM_VIEW, vCAM_TURN, 0};

int Orbit :: privat_tab[] = {vCAM_TYPE, vCAM_VIEW, vCAM_ORBIT_DIST, vCAM_TURN, 0};

int Translate :: privat_tab[] = {vCAM_DIMENSIONS, vCAM_VIEW, 0};
int Translate :: public_tab[] = {vCAMERA, 0};

int Control :: public_tab[] = {vLVM, 0};
int Control :: privat_tab[] = {vCAM_TYPE, vCAM_ORBIT_DIST, vCAM_TURN, 0};

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
 { mp_dimensions->x = m_zfar;
   mp_dimensions->y = m_znear;
   mp_dimensions->w = m_zfar / (1.f / tanf(0.5f * m_fov)); //fovY
   mp_dimensions->z = m_zfar / (mp_dimensions->w / m_aspect); //fovX
   return true;
 }

void Render :: linkVar(int def, void* data)
 { glm::ivec2 *iv2;

   switch(def)
    { case vCAM_VIEW:
       mp_transMatrix = (glm::mat4*)data;
       break;
      case vWIN_RECT:
       iv2 = (glm::ivec2*)data;
       m_aspect = (float)iv2->x / (float)iv2->y;
       m_proj = glm::perspective(m_fov, m_aspect, m_znear, m_zfar);
       m_ortho = glm::ortho(0.f, (float)iv2->x, 0.f, (float)iv2->y);
       break;
      case vCAMERA:
       (*(Render**)data) = this;
       break;
      case vCAM_DIMENSIONS:
       mp_dimensions = (glm::vec4*)data;
       break;
    }
 }


void Render :: doUpdate()
 { m_unit->lock();
   memcpy(&m_view, mp_transMatrix, sizeof(glm::mat4));
   m_unit->unlock();

   m_pvMatrix = m_proj * m_view;
 }

// --------------------------------------------------------------

Flight :: Flight(unsigned int pt) : Component(pt)
 { m_orientation = glm::quat(1.f, 0.f, 0.f, 0.f);

   privat_var = Flight::privat_tab;
   m_update = (CUpdate)&Flight::doUpdate;
   metaClass = &Instance;
 }

Flight :: ~Flight()
 { }

void Flight :: linkVar(int def, void* data)
 { switch(def)
    { case vCAM_TYPE:
       mp_type = (int*)data;
       break;
      case vCAM_VIEW:
       mp_view = (glm::mat4*)data;
       break;
      case vCAM_TURN:
       mp_turn = (glm::vec3*)data;
       break;
    }
 }

void Flight :: doUpdate()
 { m_unit->lock();
   if(*mp_type != CAM_FLIGHT)
    { m_unit->unlock();
      return;
    }
   else
    { glm::vec3 tmp(mp_turn->x, mp_turn->y, mp_turn->z);
      *mp_turn = glm::vec3(0.f);
      m_unit->unlock();

      if((tmp.x != 0.f)||(tmp.y != 0.f)||(tmp.z != 0.f))
       rotate(tmp.x * 0.005f, tmp.y * 0.005f, tmp.z);
      updateView();

      m_unit->lock();
      memcpy(mp_view, &m_view, sizeof(glm::mat4));
      m_unit->unlock();
    }
 }

void Flight :: rotate(float heading, float pitch, float roll)
 { glm::mat4 mat = glm::yawPitchRoll(-heading, -pitch, -roll);
   glm::quat rot = glm::quat_cast(mat);

   m_orientation = glm::normalize(rot * m_orientation);
 }

void Flight :: updateView()
 { m_view = glm::mat4_cast(m_orientation);

 //  m_view[3][0] = - glm::dot(glm::vec3(m_view[0][0], m_view[1][0], m_view[2][0]), pos);
 //  m_view[3][1] = - glm::dot(glm::vec3(m_view[0][1], m_view[1][1], m_view[2][1]), pos);
 //  m_view[3][2] = - glm::dot(zAxis, pos);
 }

// --------------------------------------------------------------

Orbit :: Orbit(unsigned int pt) : Component(pt)
 { m_dist = 45.f;
   m_orientation = glm::quat(1.f, 0.f, 0.f, 0.f);

   privat_var = Orbit::privat_tab;
   m_update = (CUpdate)&Orbit::doUpdate;
   metaClass = &Instance;
 }

Orbit :: ~Orbit()
 { }

void Orbit :: linkVar(int def, void* data)
 { switch(def)
    { case vCAM_TYPE:
       mp_type = (int*)data;
       break;
      case vCAM_VIEW:
       mp_view = (glm::mat4*)data;
       break;
      case vCAM_ORBIT_DIST:
       mp_dist = (float*)data;
       break;
      case vCAM_TURN:
       mp_turn = (glm::vec3*)data;
       break;
    }
 }

void Orbit :: doUpdate()
 { m_unit->lock();
   if(*mp_type != CAM_ORBIT)
    { m_unit->unlock();
      return;
    }
   else
    { m_dist += *mp_dist;
      glm::vec2 tmp(mp_turn->x, mp_turn->y);
      *mp_turn = glm::vec3(0.f);
      m_unit->unlock();

      if((tmp.x != 0.f)||(tmp.y != 0.f))
       rotate(tmp.y * 0.005f, tmp.x * 0.005f);
      updateView();

      m_unit->lock();
      memcpy(mp_view, &m_view, sizeof(glm::mat4));
      m_unit->unlock();
    }
 }

void Orbit :: rotate(float heading, float pitch)
 { pitch = -pitch;
   heading = -heading;

   glm::quat rot;

   if (heading != 0.0f)
    { rot = glm::angleAxis(heading, glm::vec3(1.f, 0.f, 0.f));
      m_orientation = rot * m_orientation;
    }
   if (pitch != 0.0f)
    { rot = glm::angleAxis(pitch, glm::vec3(0.f, 1.f, 0.f));
      m_orientation = m_orientation * rot;
    }

   m_orientation = glm::normalize(m_orientation);
 }

void Orbit :: updateView()
 { m_view = glm::mat4_cast(m_orientation);

   glm::vec3 zAxis = glm::vec3(m_view[0][2], m_view[1][2], m_view[2][2]);
   glm::vec3 pos = zAxis * m_dist;

   m_view[3][0] = - glm::dot(glm::vec3(m_view[0][0], m_view[1][0], m_view[2][0]), pos);
   m_view[3][1] = - glm::dot(glm::vec3(m_view[0][1], m_view[1][1], m_view[2][1]), pos);
   m_view[3][2] = - glm::dot(zAxis, pos);
 }

// --------------------------------------------------------------

Translate :: Translate(unsigned int pt) : Component(pt)
 { m_pos = glm::vec3(0.f);
   m_target = glm::vec3(0.f, 0.f, 0.f);

   privat_var = Translate::privat_tab;
   public_var = Translate::public_tab;
   m_update = (CUpdate)&Translate::doUpdate;
   m_id = 0;
   metaClass = &Instance;
 }

Translate :: ~Translate()
 { }

void Translate :: linkVar(int def, void* data)
 { switch(def)
    { case vCAM_DIMENSIONS:
       mp_dimensions = (glm::vec4*)data;
       break;
      case vCAMERA:
       (*(Translate**)data) = this;
       break;
      case vCAM_VIEW:
       mp_view = (glm::mat4*)data;
       break;
    }
 }

void Translate :: doUpdate()
 { m_unit->lock();
   memcpy(&m_view, mp_view, sizeof(glm::mat4));
   m_unit->unlock();

   buildFrustrum(m_view);
 }

void Translate :: buildFrustrum(glm::mat4 &matrix)
 { glm::vec3 viewDir(matrix[0][2], matrix[1][2], matrix[2][2]);
   glm::vec3 rightDir(matrix[0][0], matrix[1][0], matrix[2][0]);
   glm::vec3 upDir(matrix[0][1], matrix[1][1], matrix[2][1]);

   viewDir *= -1.f;
   glm::vec3 v(viewDir   * mp_dimensions->x);
   glm::vec3 hr(rightDir * mp_dimensions->z);
   glm::vec3 hu(upDir    * mp_dimensions->w);
   glm::vec3 vv[4];

   vv [0] = m_pos + v + hr + hu;
   vv [1] = m_pos + v - hr + hu;
   vv [2] = m_pos + v - hr - hu;
   vv [3] = m_pos + v + hr - hu;

   m_frustrum.set(m_pos, vv, 4);
   Plane pl = Plane(-viewDir, m_pos + mp_dimensions->x * viewDir);
   m_frustrum.addPlane(pl);
   pl = Plane(viewDir, m_pos + mp_dimensions->y * viewDir);
   m_frustrum.addPlane(pl);
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
       mp_turn = (glm::vec3*)data;
       break;
      case vCAM_TYPE:
       mp_type = (int*)data;
       *mp_type = CAM_ORBIT;
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

void Control :: setType(int tp)
 { m_unit->lock();
   *mp_type = tp;
   m_unit->unlock();
   if(tp == CAM_FLIGHT) LOG_SPAM("Camera::Control Camera type FLIGHT");
   else if(tp == CAM_ORBIT) LOG_SPAM("Camera::Control Camera type ORBIT");
 }

int Control :: getType()
 { int tp;
   m_unit->lock();
   tp = *mp_type;
   m_unit->unlock();
   return tp;
 }

void Control :: setDistance(float volume)
 { m_unit->lock();
   *mp_dist = volume;
   m_unit->unlock();
 }

float Control :: getDistance()
 { float dist;
   m_unit->lock();
   dist = *mp_dist;
   m_unit->unlock();
   return dist;
 }

void Control :: rotation(int x, int y)
 { if(m_flag == 1)
    { m_unit->lock();
      *mp_turn = glm::vec3((float)(x - m_cursor.x), (float)(y - m_cursor.y), 0.f);
      m_unit->unlock();
      m_cursor = glm::ivec2(x, y);
    }
 }
