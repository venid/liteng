#include "typevar.h"
#include "luastate.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "object.h"
#include "vertex.h"
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

void* setLua() { return new Lua::State(); }
void  delLua(void* vr) { delete (Lua::State*)vr; }

void* setTimer() { return new Timer(); }
void  delTimer(void* vr) { delete (Timer*)vr; }

//void* setBounding() { return new Bounding(); }
//void  delBounding(void* vr) { delete (Bounding*)vr; }

void* setOperation() { return new Operation; }
void  delOperation(void* vr) { delete (Operation*)vr; }

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
void* getVar() { return new Tp; }

template<typename Tp>
void delVar(void* vr) { delete (Tp*)vr; }

template<>
void delVar<Object**>(void* vr)
 { Object** obj = (Object**)vr;
   (*obj)->release();
   delete obj;
 }

template<typename Tp>
void* getVec() { return new std::vector<Tp>; }

template<typename Tp>
void  delVec(void* vr)
 { ((std::vector<Tp>*)vr)->clear();
   delete (std::vector<Tp>*)vr;
 }

template<>
void delVec<Object*>(void* vr)
 { for(auto it = ((std::vector<Object*>*)vr)->begin(); it != ((std::vector<Object*>*)vr)->end(); it++)
    (*it)->release();
   ((std::vector<Object*>*)vr)->clear();
   delete (std::vector<Object*>*)vr;
 }

// ---------------------------------------------------------------------------------------------

#define VAR(Tp) {getVar<Tp>, delVar<Tp>}
#define VEC(Tp) {getVec<Tp>, delVec<Tp>}

std::map<int, std::pair<void*(*)(), void(*)(void*)> > VarTable
 {{ vWIN_RECT,       {setVec2i, delVec2i}},
  { vCURSOR,         {setVec2i, delVec2i}},
  { vLVM,            {setPointer, delPointer}},
  { vTIMER,          {setTimer, delTimer}},
  { vDELTA_TIME,     VAR(double)},

  { vPROGRAM,        {setPointer, delPointer}},
  { vRES_MANAGER,    VAR(void*)},

  { vLIST_DRAW,      VAR(void*)},
  { vLIST_RENDER,    VAR(void*)},
  { vLIST_BORDER,    VAR(void*)},

  { vWORLD,          {setPointer, delPointer}},
  { vVECTOR_SEGMENT, VEC(Object*)},
  { vVECTOR_SCENE,   VEC(Object*)},

  { vCAMERA,         VAR(void*)},
  { vCAM_POS,        VAR(glm::vec3)},
  { vCAM_TRANSLATE,  {setMat4f, delMat4f}},
  { vCAM_VIEW,       {setMat4f, delMat4f}},
  { vCAM_MATRIX,     {setMat4f, delMat4f}},
  { vCAM_DIMENSIONS, {setVec4f, delVec4f}},
  { vCAM_ORBIT_DIST, {setFloat, delFloat}},
  { vCAM_TURN,       {setVec3f, delVec3f}},
  { vCAM_CONTROL,    {setInt, delInt}},
  { vCAM_TYPE,       {setInt, delInt}},
  { vCAM_CURSOR,     VAR(glm::vec2)},

  { vTRANSLATE,      {setMat4f, delMat4f}},
  { vOPERATION,      {setOperation, delOperation}},
//  { vORIENTATION,    {setQuat, delQuat}},
  { vPOSITION,       {setVec3f, delVec3f}},
  { vMESH,           {setPointer, delPointer}},
  { vMATERIAL,       {setPointer, delPointer}},
//  { vBOUNDING,       {setBounding, delBounding}},

  //{_FBO_GEOM_,       getFBO},
  //{_FBO_COLOR_,      getFBO},
  //{_FBO_LIGHT_,      getFBO},

  {vGUI_COLOR_WIN,   {setVec4f, delVec4f}}
 };
