
#pragma once

#include <GL/gl.h>
#include <glm/glm.hpp>

#define BOX_00 0x00000001
#define BOX_01 0x00000002
#define BOX_02 0x00000004
#define BOX_03 0x00000008
#define BOX_04 0x00000010 //norm 5
#define BOX_05 0x00000020
#define BOX_06 0x00000040
#define BOX_07 0x00000080
#define BOX_08 0x00000100
#define BOX_09 0x00000200
#define BOX_10 0x00000400  //norm 1
#define BOX_11 0x00000800
#define BOX_12 0x00001000  //norm 3
#define BOX_13 0x00002000
#define BOX_14 0x00004000  //norm 2
#define BOX_15 0x00008000
#define BOX_16 0x00010000  //norm 0
#define BOX_17 0x00020000
#define BOX_18 0x00040000
#define BOX_19 0x00080000
#define BOX_20 0x00100000
#define BOX_21 0x00200000
#define BOX_22 0x00400000  //norm 4
#define BOX_23 0x00800000
#define BOX_24 0x01000000
#define BOX_25 0x02000000
#define BOX_26 0x04000000
#define BOX_FF 0x80000000


struct Vertex
 { glm::vec3 pos;
   glm::vec3 norm;
   glm::vec2 coord;
   glm::vec4 color;
   glm::vec3 tangent;
   glm::vec3 binormal;
 };

struct Face
 { unsigned int index[3]; };

struct packFace
 { glm::uvec2 i1;
   glm::uvec2 i2;
   glm::uvec2 i3;
 };

inline const GLubyte *BUFFER_OFFSET(size_t bytes)
{ return reinterpret_cast<const GLubyte *>(0) + bytes; }

class Mesh;
class Material;
struct Generic
 { unsigned int id;
   Mesh *mesh;
   Material *mat;
   glm::mat4 transform;
   Generic *next;
 };
