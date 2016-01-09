
#include <memory.h>
#include <glm/gtx/normal.hpp>
#include <GL/gl.h>
#include <GL/glext.h>
#include <cmath>
#include <vector>
#include "glfun.h"
#include "mesh.h"

#include "log.h"

// ---------------------------------------------------------------------------------------------
glm::mat3 textureCoordMatrix(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3);

glm::vec3 ClosestPointOnLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& p);
glm::vec3 Ortogonalize(const glm::vec3& v1, const glm::vec3& v2);
void CalcTriangleBasis( const glm::vec3& E, const glm::vec3& F, const glm::vec3& G,
                        float sE, float tE, float sF, float tF, float sG, float tG,
                        glm::vec3& tangentX, glm::vec3& tangentY);
// ----------------------------------------------------------------------------------------------

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
   numComp = 0;
   typeGL = 0;
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
   numComp = 0;
   typeGL = 0;
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
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, numF * numComp * sizeof(int), face, GL_STATIC_DRAW);
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
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, szFace * numComp * sizeof(int), nullptr, GL_STATIC_DRAW);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, szFace * numComp * sizeof(int), face);
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
   if(face == nullptr) glDrawArrays(typeGL, 0, szVert);
   else
    { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
      glDrawElements(GL_TRIANGLES, szFace * numComp, GL_UNSIGNED_INT, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
 }

void Mesh :: setVertex(Vertex *vertex, unsigned int sz, unsigned int type)
 { if(vert) delete [] vert;
   numV = sz;
   vert = vertex;
   typeGL = type;
   actual |= VERT_MESH;
 }

void Mesh :: setFace(Face *faces, unsigned int sz, unsigned char num)
 { if(face) delete [] face;
   numF = sz;
   face = faces;
   numComp = num;
   actual |= FACE_MESH;
 }

void Mesh :: getDimensions(glm::vec3 &vmin, glm::vec3 &vmax)
 { vmin = vert[0].pos;
   vmax = vert[0].pos;

   for(unsigned int i = 1; i < numV; i++)
    { if(vert[i].pos.x > vmax.x) vmax.x = vert[i].pos.x;
      else if(vert[i].pos.x < vmin.x) vmin.x = vert[i].pos.x;

      if(vert[i].pos.y > vmax.y) vmax.y = vert[i].pos.y;
      else if(vert[i].pos.y < vmin.y) vmin.y = vert[i].pos.y;

      if(vert[i].pos.z > vmax.z) vmax.z = vert[i].pos.z;
      else if(vert[i].pos.z < vmin.z) vmin.z = vert[i].pos.z;
    }
 }

void Mesh :: computeNormals()
 { unsigned int k;
   glm::vec3 nv;

   if(face)
    { for (k=0; k < numV; k++)
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
   else
    { for (k = 0; k < numV;)
       { nv = glm::triangleNormal(vert[k].pos,
                                  vert[k + 1].pos,
                                  vert[k + 2].pos);
         vert[k].norm = nv;
         vert[k + 1].norm = nv;
         vert[k + 2].norm = nv;
         k += 3;
       }
    }
 }

void Mesh :: computeTangents()
 { if(face) tangentFace();
   else tangentVertex();
 }

void Mesh :: computeTextureCoord(float scale)
 { unsigned int i = 0;
   glm::vec3 vt;
   glm::mat3 rot;

   if(face == nullptr)
    while(i < numV)
     { rot = textureCoordMatrix(vert[i].pos, vert[i + 1].pos, vert[i + 2].pos);

       vt = rot * vert[i].pos * scale;
       vert[i].coord = glm::vec2(vt.x, vt.y);
       vt = rot * vert[i + 1].pos * scale;
       vert[i + 1].coord = glm::vec2(vt.x, vt.y);
       vt = rot * vert[i + 2].pos * scale;
       vert[i + 2].coord = glm::vec2(vt.x, vt.y);

       i += 3;
     }
 }

void Mesh :: tangentFace()
 { register unsigned int i, j;
   glm::vec3* tangents = new glm::vec3[numF];
   glm::vec3* binormals = new glm::vec3[numF];

   for (i = 0; i < numF; i++ )
    { int ind0 = face[ i ].index[0];
      int ind1 = face[ i ].index[1];
      int ind2 = face[ i ].index[2];
      float s1 = vert[ ind0 ].coord.x;
      float t1 = vert[ ind0 ].coord.y;
      float s2 = vert[ ind1 ].coord.x;
      float t2 = vert[ ind1 ].coord.y;
      float s3 = vert[ ind2 ].coord.x;
      float t3 = vert[ ind2 ].coord.y;

      glm::vec3  t, b;
      CalcTriangleBasis( vert[face[i].index[0]].pos,
                         vert[face[i].index[1]].pos,
                         vert[face[i].index[2]].pos,
                         s1, t1, s2, t2, s3, t3, t, b );
      tangents[i] = t;
      binormals[i] = b;
    }

   // теперь пройдемся по всем вершинам, для каждой из них найдем
   // грани ее содержащие, и запишем все это хозяйство на будущее =)
   std::vector<glm::vec3> rt, rb;
   for ( i = 0; i < numV; i++ )
    { for ( j = 0; j < numF; j++ )
       if ( face[j].index[0] == i || face[j].index[1] == i || face[j].index[2] == i )
        { // нашли грань которой эта вершина принадлежит.
          // добавим вектора этой грани в наш массив
          rt.push_back( tangents[ j ] );
          rb.push_back( binormals[ j ] );
        }

      // все прилежащие вектора нашли, теперь просуммируем их
      // и разделим на их количество, т.е. усредним.
      glm::vec3 tangentRes(0.f, 0.f, 0.f);
      glm::vec3 binormalRes(0.f, 0.f, 0.f);
      for ( j = 0; j < rt.size(); j++ )
       { tangentRes += rt[ j ];
         binormalRes += rb[ j ];
       }
      tangentRes /= float( rt.size() );
      binormalRes /= float( rb.size() );
      rt.clear();
      rb.clear();
      // а теперь то, о чем многие забывают. Как мы помним,
      // TBN базис представляет собой всего-навсего систему координат.
      // поэтому все три направляющие вектора этого базиса
      // обязаны быть попарно перпендикулярны. Вот об этом мы
      // сейчас и побеспокоимся, выполнив ортогонализацию
      // векторов методом Грама-Шмидта
      vert[i].tangent = Ortogonalize(vert[i].norm, tangentRes);
      vert[i].binormal = Ortogonalize(vert[i].norm, binormalRes);

    }
    delete [] tangents;
    delete [] binormals;
 }

void Mesh :: tangentVertex()
 {

 }

// ----------------------------------------------------------------------------

glm::mat3 textureCoordMatrix(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3)
 { glm::vec3 rot_x, rot_y, rot_z; //Базисные вектора матрицы поворота.

   rot_z = glm::triangleNormal(v1, v2, v3);
   rot_x = glm::normalize(v2 - v1);
   rot_y = glm::cross(rot_z, rot_x);
   return glm::inverse(glm::mat3( rot_x, rot_y, rot_z ));
 }


Mesh* Mesh :: makeBox(float x, float y, float z)
 { Mesh* ms = new Mesh("");
   Vertex* vrt = new Vertex[24];
   Face* fc =  new Face[12];
   glm::vec3 hsz = glm::vec3(x*0.5f, y*0.5f, z*0.5f);
   glm::vec3 tmp;
   glm::mat3 txm;

   vrt[0].pos = glm::vec3(-hsz.x, hsz.y, hsz.z);
   vrt[1].pos = glm::vec3(hsz.x, hsz.y, hsz.z);
   vrt[2].pos = glm::vec3(hsz.x, -hsz.y, hsz.z);
   vrt[3].pos = glm::vec3(-hsz.x, -hsz.y, hsz.z);
   txm = textureCoordMatrix(vrt[0].pos, vrt[1].pos, vrt[2].pos);
   tmp = txm * vrt[0].pos; vrt[0].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[1].pos; vrt[1].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[2].pos; vrt[2].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[3].pos; vrt[3].coord = glm::vec2(tmp.x, tmp.y);


   vrt[4].pos = glm::vec3(-hsz.x, hsz.y, -hsz.z);
   vrt[5].pos = glm::vec3(hsz.x, hsz.y, -hsz.z);
   vrt[6].pos = glm::vec3(hsz.x, -hsz.y, -hsz.z);
   vrt[7].pos = glm::vec3(-hsz.x, -hsz.y, -hsz.z);
   txm = textureCoordMatrix(vrt[4].pos, vrt[5].pos, vrt[6].pos);
   tmp = txm * vrt[4].pos; vrt[4].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[5].pos; vrt[5].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[6].pos; vrt[6].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[7].pos; vrt[7].coord = glm::vec2(tmp.x, tmp.y);

   vrt[8].pos = glm::vec3(-hsz.x, hsz.y, -hsz.z);
   vrt[9].pos = glm::vec3(hsz.x, hsz.y, -hsz.z);
   vrt[10].pos = glm::vec3(hsz.x, hsz.y, hsz.z);
   vrt[11].pos = glm::vec3(-hsz.x, hsz.y, hsz.z);
   txm = textureCoordMatrix(vrt[8].pos, vrt[9].pos, vrt[10].pos);
   tmp = txm * vrt[8].pos; vrt[8].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[9].pos; vrt[9].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[10].pos; vrt[10].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[11].pos; vrt[11].coord = glm::vec2(tmp.x, tmp.y);

   vrt[12].pos = glm::vec3(-hsz.x, -hsz.y, -hsz.z);
   vrt[13].pos = glm::vec3(hsz.x, -hsz.y, -hsz.z);
   vrt[14].pos = glm::vec3(hsz.x, -hsz.y, hsz.z);
   vrt[15].pos = glm::vec3(-hsz.x, -hsz.y, hsz.z);
   txm = textureCoordMatrix(vrt[12].pos, vrt[13].pos, vrt[14].pos);
   tmp = txm * vrt[12].pos; vrt[12].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[13].pos; vrt[13].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[14].pos; vrt[14].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[15].pos; vrt[15].coord = glm::vec2(tmp.x, tmp.y);

   vrt[16].pos = glm::vec3(hsz.x, -hsz.y, -hsz.z);
   vrt[17].pos = glm::vec3(hsz.x, hsz.y, -hsz.z);
   vrt[18].pos = glm::vec3(hsz.x, hsz.y, hsz.z);
   vrt[19].pos = glm::vec3(hsz.x, -hsz.y, hsz.z);
   txm = textureCoordMatrix(vrt[16].pos, vrt[17].pos, vrt[18].pos);
   tmp = txm * vrt[16].pos; vrt[16].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[17].pos; vrt[17].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[18].pos; vrt[18].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[19].pos; vrt[19].coord = glm::vec2(tmp.x, tmp.y);

   vrt[20].pos = glm::vec3(-hsz.x, -hsz.y, -hsz.z);
   vrt[21].pos = glm::vec3(-hsz.x, -hsz.y, hsz.z);
   vrt[22].pos = glm::vec3(-hsz.x, hsz.y, hsz.z);
   vrt[23].pos = glm::vec3(-hsz.x, hsz.y, -hsz.z);
   txm = textureCoordMatrix(vrt[20].pos, vrt[21].pos, vrt[22].pos);
   tmp = txm * vrt[20].pos; vrt[20].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[21].pos; vrt[21].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[22].pos; vrt[22].coord = glm::vec2(tmp.x, tmp.y);
   tmp = txm * vrt[23].pos; vrt[23].coord = glm::vec2(tmp.x, tmp.y);
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
   ms->computeTangents();
   return ms;
 }

/// ----------------------------------------------------------------------------------------------

glm::vec3 ClosestPointOnLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& p)
 { glm::vec3 c = p - a;
   glm::vec3 V = b - a;
   float d = glm::length(V);

   V = glm::normalize(V);
   float t = glm::dot(V, c); // скалярное произведение векторов

   // проверка на выход за границы отрезка
   if ( t < 0.0f ) return a;
   if ( t > d ) return b;

   // Вернем точку между a и b
   V *= t;
   return ( a + V );
 }

glm::vec3 Ortogonalize( const glm::vec3& v1, const glm::vec3& v2 )
 { glm::vec3 v2ProjV1 = ClosestPointOnLine( v1, -v1, v2 );
   return glm::normalize(v2 - v2ProjV1);
 }

void CalcTriangleBasis( const glm::vec3& E, const glm::vec3& F, const glm::vec3& G,
                        float sE, float tE, float sF, float tF, float sG, float tG,
                        glm::vec3& tangentX, glm::vec3& tangentY)
 { glm::vec3 P = F - E;
   glm::vec3 Q = G - E;
   float s1 = sF - sE;
   float t1 = tF - tE;
   float s2 = sG - sE;
   float t2 = tG - tE;
   float pqMatrix[2][3];
   pqMatrix[0][0] = P[0];
   pqMatrix[0][1] = P[1];
   pqMatrix[0][2] = P[2];
   pqMatrix[1][0] = Q[0];
   pqMatrix[1][1] = Q[1];
   pqMatrix[1][2] = Q[2];
   float temp = 1.0f / ( s1 * t2 - s2 * t1);
   float stMatrix[2][2];
   stMatrix[0][0] =  t2 * temp;
   stMatrix[0][1] = -t1 * temp;
   stMatrix[1][0] = -s2 * temp;
   stMatrix[1][1] =  s1 * temp;
   float tbMatrix[2][3];
   tbMatrix[0][0] = stMatrix[0][0] * pqMatrix[0][0] + stMatrix[0][1] * pqMatrix[1][0];
   tbMatrix[0][1] = stMatrix[0][0] * pqMatrix[0][1] + stMatrix[0][1] * pqMatrix[1][1];
   tbMatrix[0][2] = stMatrix[0][0] * pqMatrix[0][2] + stMatrix[0][1] * pqMatrix[1][2];
   tbMatrix[1][0] = stMatrix[1][0] * pqMatrix[0][0] + stMatrix[1][1] * pqMatrix[1][0];
   tbMatrix[1][1] = stMatrix[1][0] * pqMatrix[0][1] + stMatrix[1][1] * pqMatrix[1][1];
   tbMatrix[1][2] = stMatrix[1][0] * pqMatrix[0][2] + stMatrix[1][1] * pqMatrix[1][2];
   tangentX = glm::normalize(glm::vec3(tbMatrix[0][0], tbMatrix[0][1], tbMatrix[0][2]));
   tangentY = glm::normalize(glm::vec3(tbMatrix[1][0], tbMatrix[1][1], tbMatrix[1][2]));
 }
