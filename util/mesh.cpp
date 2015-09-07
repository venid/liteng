
#include <memory.h>
#include <glm/gtx/normal.hpp>
#include <GL/gl.h>
#include <GL/glext.h>
#include <cmath>
#include "glfun.h"
#include "mesh.h"

#include "log.h"

META_METHODS(Mesh)
META_PROPERTY(Mesh)
META_OBJECT(Mesh, Mesh, &Object::Instance)

Mesh :: Mesh() : Object()
 { m_init = 0;
   numV = 0;
   numF = 0;
   vert = nullptr;
   face = nullptr;
   vBuffer = 0;
   iBuffer = 0;
   aBuffer = 0;
   szVert = 0;
   szFace = 0;
   actual = VERT_MESH | FACE_MESH;
   metaClass = &Instance;
 }

Mesh :: Mesh(const char *theName) : Object(theName)
 { m_init = 0;
   numV = 0;
   numF = 0;
   vert = nullptr;
   face = nullptr;
   vBuffer = 0;
   iBuffer = 0;
   aBuffer = 0;
   szVert = 0;
   szFace = 0;
   actual = VERT_MESH | FACE_MESH;
   metaClass = &Instance;
 }

Mesh :: ~Mesh()
 { if(vert) delete [] vert;
   if(face) delete [] face;
 }

bool Mesh :: init()
 { GLuint attr = 0;
   if(m_init == 0)
    { lock();
      szVert = numV;
      szFace = numF;
      actual = true;
      // create vertex data VBO
      glGenBuffers(1, &vBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
      glBufferData(GL_ARRAY_BUFFER, numV * sizeof(Vertex), vert, GL_STREAM_DRAW);
      // create index data VBO
      if(face != nullptr)
       { glGenBuffers(1, &iBuffer);
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, numF * 3 * sizeof(int), face, GL_STATIC_DRAW);
       }
      unlock();
      // create data VAO
      glGenVertexArrays(1, &aBuffer);
      glBindVertexArray(aBuffer);

      glEnableVertexAttribArray(attr);
      glVertexAttribPointer(attr, 3, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(0));
      attr = 1;
      glEnableVertexAttribArray(attr);
      glVertexAttribPointer(attr, 3, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));
      attr = 2;
      glEnableVertexAttribArray(attr);
      glVertexAttribPointer(attr, 2, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 6));

      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      LOG_DEBUG("MESH: create vbo, vao complit.");
    }
   m_init ++;
   return true;
 }

bool Mesh :: update()
 { bool result = false;
   lock();
   if((actual & VERT_MESH) == VERT_MESH)
    { szVert = numV;
      glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
      glBufferData(GL_ARRAY_BUFFER, szVert * sizeof(Vertex), nullptr, GL_STREAM_DRAW);
      glBufferSubData(GL_ARRAY_BUFFER, 0, szVert * sizeof(Vertex), vert);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      actual ^= VERT_MESH;
      result = true;
    }
   if((actual & FACE_MESH) == FACE_MESH)
    { szFace = numF;
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, szFace * 3 * sizeof(int), nullptr, GL_STATIC_DRAW);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, szFace * 3 * sizeof(int), face);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      actual ^= FACE_MESH;
      result = true;
    }
   unlock();
   return result;
 }

void Mesh :: clear()
 { m_init --;
   if(m_init == 0)
    { glDeleteBuffers(1, &vBuffer);
      if(face != nullptr) glDeleteBuffers(1, &iBuffer);
      glDeleteVertexArrays(1, &aBuffer);
      LOG_DEBUG("MESH: vbo, vao clear.");
    }
 }

void Mesh :: render()
 { unsigned int bId = 0;

   glBindVertexArray(aBuffer);
   glBindVertexBuffer(bId, vBuffer, 0, sizeof(Vertex));
   if(face == nullptr) glDrawArrays(GL_TRIANGLES, 0, szVert);
   else
    { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
      glDrawElements(GL_TRIANGLES, szFace * 3, GL_UNSIGNED_INT, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
 }

void Mesh :: setVertex(Vertex *vertex, unsigned int sz)
 { if(vert) delete [] vert;
   numV = sz;
   vert = vertex;
   actual |= VERT_MESH;
 }

void Mesh :: setFace(Face *faces, unsigned int sz)
 { if(face) delete [] face;
   numF = sz;
   face = faces;
   actual |= FACE_MESH;
 }

void Mesh :: computeNormals()
 { unsigned int k;
   glm::vec3 nv;

   for (k=0; k < numV; k++)
     vert[k].norm = glm::vec3(0.f);

   k = 0;
   while (k < numF)
    { nv = glm::triangleNormal(vert[face[k].index[0]].pos,
                               vert[face[k].index[1]].pos,
                               vert[face[k].index[2]].pos);
      vert[face[k].index[0]].norm += nv;
      vert[face[k].index[1]].norm += nv;
      vert[face[k].index[2]].norm += nv;
      k++;
    }
   for (k=0; k < numV; k++)
     vert[k].norm = glm::normalize(vert[k].norm);
 }

// ------------------------------------------------------------------------

Mesh* Mesh :: makeBox(float x, float y, float z)
 { Mesh* ms = new Mesh("");
   Vertex* vrt = new Vertex[24];
   Face* fc =  new Face[12];
   glm::vec3 hsz = glm::vec3(x*0.5f, y*0.5f, z*0.5f);

   vrt[0].pos = glm::vec3(-hsz.x, hsz.y, hsz.z);  vrt[0].coord = glm::vec2(0.f, 1.f);
   vrt[1].pos = glm::vec3(hsz.x, hsz.y, hsz.z);   vrt[1].coord = glm::vec2(1.f, 1.f);
   vrt[2].pos = glm::vec3(hsz.x, -hsz.y, hsz.z);  vrt[2].coord = glm::vec2(1.f, 0.f);
   vrt[3].pos = glm::vec3(-hsz.x, -hsz.y, hsz.z); vrt[3].coord = glm::vec2(0.f, 0.f);

   vrt[4].pos = glm::vec3(-hsz.x, hsz.y, -hsz.z); vrt[4].coord = glm::vec2(0.f, 0.f);
   vrt[5].pos = glm::vec3(hsz.x, hsz.y, -hsz.z);  vrt[5].coord = glm::vec2(1.f, 0.f);
   vrt[6].pos = glm::vec3(hsz.x, -hsz.y, -hsz.z); vrt[6].coord = glm::vec2(1.f, 1.f);
   vrt[7].pos = glm::vec3(-hsz.x, -hsz.y, -hsz.z); vrt[7].coord = glm::vec2(0.f, 1.f);

   vrt[8].pos = glm::vec3(-hsz.x, hsz.y, -hsz.z); vrt[8].coord = glm::vec2(0.f, 0.f);
   vrt[9].pos = glm::vec3(hsz.x, hsz.y, -hsz.z);  vrt[9].coord = glm::vec2(1.f, 0.f);
   vrt[10].pos = glm::vec3(hsz.x, hsz.y, hsz.z);  vrt[10].coord = glm::vec2(1.f, 1.f);
   vrt[11].pos = glm::vec3(-hsz.x, hsz.y, hsz.z); vrt[11].coord = glm::vec2(0.f, 1.f);

   vrt[12].pos = glm::vec3(-hsz.x, -hsz.y, -hsz.z); vrt[12].coord = glm::vec2(0.f, 0.f);
   vrt[13].pos = glm::vec3(hsz.x, -hsz.y, -hsz.z);  vrt[13].coord = glm::vec2(1.f, 0.f);
   vrt[14].pos = glm::vec3(hsz.x, -hsz.y, hsz.z);   vrt[14].coord = glm::vec2(1.f, 1.f);
   vrt[15].pos = glm::vec3(-hsz.x, -hsz.y, hsz.z);  vrt[15].coord = glm::vec2(0.f, 1.f);

   vrt[16].pos = glm::vec3(hsz.x, -hsz.y, -hsz.z);  vrt[16].coord = glm::vec2(0.f, 0.f);
   vrt[17].pos = glm::vec3(hsz.x, hsz.y, -hsz.z);   vrt[17].coord = glm::vec2(1.f, 0.f);
   vrt[18].pos = glm::vec3(hsz.x, hsz.y, hsz.z);    vrt[18].coord = glm::vec2(1.f, 1.f);
   vrt[19].pos = glm::vec3(hsz.x, -hsz.y, hsz.z);   vrt[19].coord = glm::vec2(0.f, 1.f);

   vrt[20].pos = glm::vec3(-hsz.x, -hsz.y, -hsz.z); vrt[20].coord = glm::vec2(0.f, 0.f);
   vrt[21].pos = glm::vec3(-hsz.x, -hsz.y, hsz.z);  vrt[21].coord = glm::vec2(1.f, 0.f);
   vrt[22].pos = glm::vec3(-hsz.x, hsz.y, hsz.z);   vrt[22].coord = glm::vec2(1.f, 1.f);
   vrt[23].pos = glm::vec3(-hsz.x, hsz.y, -hsz.z);  vrt[23].coord = glm::vec2(0.f, 1.f);
   ms->setVertex(vrt, 24);

   fc[0].index[0] = 0; fc[0].index[1] = 3; fc[0].index[2] = 2;
   fc[1].index[0] = 2; fc[1].index[1] = 1; fc[1].index[2] = 0;

   fc[2].index[0] = 4; fc[2].index[1] = 5; fc[2].index[2] = 6;
   fc[3].index[0] = 6; fc[3].index[1] = 7; fc[3].index[2] = 4;

   fc[4].index[0] = 8; fc[4].index[1] = 11; fc[4].index[2] = 10;
   fc[5].index[0] = 10; fc[5].index[1] = 9; fc[5].index[2] = 8;

   fc[6].index[0] = 12; fc[6].index[1] = 13; fc[6].index[2] = 14;
   fc[7].index[0] = 14; fc[7].index[1] = 15; fc[7].index[2] = 12;

   fc[8].index[0] = 16; fc[8].index[1] = 17; fc[8].index[2] = 18;
   fc[9].index[0] = 18; fc[9].index[1] = 19; fc[9].index[2] = 16;

   fc[10].index[0] = 20; fc[10].index[1] = 21; fc[10].index[2] = 22;
   fc[11].index[0] = 22; fc[11].index[1] = 23; fc[11].index[2] = 20;
   ms->setFace(fc, 12);
   ms->computeNormals();
   return ms;
 }
