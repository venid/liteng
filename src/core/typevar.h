#ifndef TYPEVAR_H_INCLUDED
#define TYPEVAR_H_INCLUDED

#include <map>
#include <utility>

struct Link
 { int refCount;
   void* data;
 };
 
// ----------------- константы -------------------------

#define MAX_ITERATION 5
#define CONST_DELTA_TIME 0.05

// --------- Типы переменных --------------

#define vWIN_RECT    0x00000010
#define vCURSOR      0x00000011
#define vLVM         0x00000012
#define vTIMER       0x00000013
#define vDELTA_TIME  0x00000014

#define vPROGRAM        0x00000021
#define vRES_MANAGER    0x00000022
#define vLIST_DRAW      0x00000023
#define vLIST_RENDER    0x00000024
#define vLIST_BORDER    0x00000025

#define vWORLD          0x00000030
#define vVECTOR_SEGMENT 0x00000031
#define vVECTOR_SCENE   0x00000032

#define vCAMERA         0x00000040
#define vCAM_POS        0x00000041
#define vCAM_TRANSLATE  0x00000042
#define vCAM_MATRIX     0x00000043
#define vCAM_VIEW       0x00000044
#define vCAM_DIMENSIONS 0x00000045
#define vCAM_ORBIT_DIST 0x00000046
#define vCAM_TURN       0x00000047
#define vCAM_CONTROL    0x00000048
#define vCAM_TYPE       0x00000049

#define vTRANSLATE      0x00000051
#define vOPERATION      0x00000052
#define vORIENTATION    0x00000053
#define vPOSITION       0x00000054
#define vMESH           0x00000055
#define vMATERIAL       0x00000056
#define vBOUNDING       0x00000057

#define vFBO_GEOM       0x00000061
#define vFBO_COLOR      0x00000062
#define vFBO_LIGHT      0x00000063

#define vGUI_COLOR_WIN  0x00000081

struct Operation
 { int flag;
   int type;
   int param;
 };

void* setInt(int val);
void* setVec2i(int x, int y);
void* setPointer(void*);
void* setFloat(float vol);

extern std::map<int, std::pair<void*(*)(), void(*)(void*)> > VarTable;

#endif // TYPEVAR_H_INCLUDED
