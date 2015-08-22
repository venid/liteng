#include "typevar.h"
#include "luastate.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "object.h"
//#include "vertex.h"
//#include "frustrum.h"
#include "timer.h"
//#include "bounding.h"
//#include "fbuffer.h"
#include <vector>

void* setInt() { return new int(0); }
void* setInt(int val) { return new int(val); }
void  delInt(void* vr) { delete (int*)vr; }

void* setFloat() { return new float(0.f); }
void* setFloat(float vol) { return new float(vol); }
void  delFloat(void* vr) { delete (float*)vr; }

void* setDouble() { return new double(0.0); }
void  delDouble(void* vr) { delete (double*)vr; }

void* setVec2i() { return new glm::ivec2(0); }
void* setVec2i(int x, int y) { return new glm::ivec2(x, y); }
void  delVec2i(void* vr) { delete (glm::ivec2*)vr; }

void* setVec2f() { return new glm::vec2(0.f); }
void  delVec2f(void* vr) { delete (glm::vec2*)vr; }

void* setVec3f() { return new glm::vec3(0.f); }
void  delVec3f(void* vr) { delete (glm::vec3*)vr; }

void* setVec4f() { return new glm::vec4(0.f); }
void  delVec4f(void* vr) { delete (glm::vec4*)vr; }

void* setMat4f() { return new glm::mat4(1.f); }
void  delMat4f(void* vr) { delete (glm::mat4*)vr; }

void* setQuat() { return new glm::quat(1.f, 0.f, 0.f, 0.f); }
void  delQuat(void* vr) { delete (glm::quat*)vr; }

//void* setFrustrum() { return new Frustrum(); }
//void  delFrustrum(void* vr) { delete (Frustrum*)vr; }

//void* setGeneric()
 //{ Generic** pv = new Generic*;
   //*pv = nullptr;
   //return pv;
 //}
//void  delGeneric(void* vr) { delete (Generic**)vr; }

void* setLua() { return new Lua::State(); }
void  delLua(void* vr) { delete (Lua::State*)vr; }

void* setTimer() { return new Timer(); }
void  delTimer(void* vr) { delete (Timer*)vr; }

//void* setBounding() { return new Bounding(); }
//void  delBounding(void* vr) { delete (Bounding*)vr; }

void* setPointer()
 { void** vd = new void*;
   *vd = nullptr;
   return vd;
 }
void* setPointer(void* vr)
 { void** vd = new void*;
   *vd = vr;
   return vd;
 }
void  delPointer(void* vr) { delete (void**)vr; }

void* setPObject()
 { Object** obj = new Object*;
   *obj = nullptr;
   return obj;
 }

void  delPObject(void* vr)
 { Object** obj = (Object**)vr;
   (*obj)->release();
   delete obj;
 }

template<typename Tp>
void* setVector() { return new std::vector<Tp>; }

template<typename Tp>
void  delVector(void* vr)
 { ((std::vector<Tp>*)vr)->clear();
   delete (std::vector<Tp>*)vr;
 }

// -----------------------------------------------------------------------------------------------

std::map<int, std::pair<void*(*)(), void(*)(void*)> > VarTable
 {{ vWIN_RECT,       {setVec2i, delVec2i}},
  { vCURSOR,         {setVec2i, delVec2i}},
  { vLVM,            {setPointer, delPointer}},
  { vTIMER,          {setTimer, delTimer}},
  { vDELTA_TIME,     {setDouble, delDouble}},

  { vPROGRAM,        {setPointer, delPointer}},
  { vRES_MANAGER,    {setPointer, delPointer}},

//  { vLIST_RENDER,    {setGeneric, delGeneric}},
//  { vVECTOR_SHAPE,   {setVector, delVector}},
//  { vVECTOR_LIGHT,   {setVector, delVector}},
//  { vVECTOR_MESH,    {setVector, delVector}},

  { vVECTOR_REGION,    {setVector<Object*>, delVector<Object*>}},

  { vCAMERA,         {setPointer, delPointer}},
  { vCAM_POS,        {setVec3f, delVec3f}},
  { vCAM_TRANSLATE,  {setMat4f, delMat4f}},
  { vCAM_VIEW,       {setMat4f, delMat4f}},
  { vCAM_MATRIX,     {setMat4f, delMat4f}},
//  { vCAM_FRUSTRUM,   {setFrustrum, delFrustrum}},
  { vCAM_ORBIT_DIST, {setFloat, delFloat}},
  { vCAM_TURN,       {setVec2f, delVec2f}},
  { vCAM_CONTROL,    {setInt, delInt}},

  { vTRANSLATE,      {setMat4f, delMat4f}},
  { vMESH,           {setPointer, delPointer}},
  { vORIENTATION,    {setQuat, delQuat}},
  { vPOSITION,       {setVec3f, delVec3f}},
  { vMATERIAL,       {setPointer, delPointer}},
//  { vBOUNDING,       {setBounding, delBounding}},

  //{_FBO_GEOM_,       getFBO},
  //{_FBO_COLOR_,      getFBO},
  //{_FBO_LIGHT_,      getFBO},

  {vGUI_COLOR_WIN,   {setVec4f, delVec4f}}
 };
