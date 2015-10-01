
#include "ship.h"
#include "unit.h"
#include "log.h"
#include "typevar.h"
#include "luastate.h"
#include "resmanager.h"
#include "segment.h"
#include "shape.h"
#include "mesh.h"
#include "vertex.h"
#include "timer.h"

using namespace Ship;

META_METHODS(Make,
 METHOD(create, Make::Create))
META_PROPERTY(Make)
META_OBJECT(Make, Ship::Make, &Component::Instance)

int Make :: privat_tab[] = {vVECTOR_SEGMENT, vTRANSLATE, 0};
int Make :: public_tab[] = {vRES_MANAGER, 0};

// ---------------------------------------------------------------

Make :: Make(Lua::Var &tab, unsigned int pt) : Component(pt)
 { m_blueprint = (std::string)tab["blueprint"];
   m_update = (CUpdate) &Make::doUpdate;
   privat_var = Make::privat_tab;
   public_var = Make::public_tab;
   m_id = 0;
   metaClass = &Instance;
 }

void Make :: linkVar(int def, void* data)
 { switch(def)
    { case vVECTOR_SEGMENT:
       mp_segment = (std::vector<Segment*>*)data;
       break;
      case vRES_MANAGER:
       mp_res = (*(ResManager**)data);
       break;
      case vTRANSLATE:
       mp_trans = (glm::mat4*)data;
       break;
    }
 }

bool Make :: init()
 { Shape* shp;
   Segment* sgm;
   Mesh* mesh;
   glm::vec3 ps(0.f);
   Timer tmr;

   Data *data = mp_res->getResource(m_blueprint);
   mp_res->lvm.load_data(data);

   mesh = new Mesh(m_blueprint.c_str());

   tmr.start();
   primary_form(ps);
   unsigned int n = set_flags();
   build(mesh, n);
   tmr.stop();
   LOG_INFO("Ship::Make: количество полигонов = %i", n);
   LOG_INFO("Ship::Make: время генерации = %f", tmr.getSampleTime());
   mp_res->regObject(mesh);

   m_unit->lock();
   sgm = new Segment();
   shp = new Shape("test_01");
   shp->setMaterial((Material*)mp_res->getObject("mt02.mat"));
   shp->setMesh((Mesh*)mp_res->getObject(m_blueprint.c_str()));
   sgm->addShape(shp);
   m_unit->unlock();

   mp_segment->push_back(sgm);

   glm::mat4 mt(1.f);
   mt[3][0] = 1.5f;
   mt[3][1] = 1.15f;
   mt[3][2] = 0.2f;
   memcpy(mp_trans, &mt, sizeof(glm::mat4));

   return true;
 }

void Make :: doUpdate()
 { }

// ----------------------------------------------------------------------------

char faceTab[6][7] = { { 1,1,1,1,1,1, 0},
                       { 2,5,1,0,1,0, 2},
                       { 2,0,5,0,3,0, 1},
                       { 2,0,5,0,1,0, 2},
                       { 1,5,1,2,1,3, 1},
                       { 4,5,3,2,5,3, 2}
                     };

Make::Cell frame[66][66][66];

// ----------------------------------------------------------------------------

void Make :: primary_form(glm::vec3 &offset)
 { int x, y, z;
   glm::ivec3 rot;
   Lua::Var tab = mp_res->lvm["frame"];
   Lua::Var tmp;
   int n = tab.size();

   memset(frame, 0, sizeof(frame));

   for(int i = 1; i <= n; i++)
    { tmp = tab[i];
      x = tmp[2];
      y = tmp[3];
      z = tmp[4];
      rot.x = tmp[5];
      rot.y = tmp[6];
      rot.z = tmp[7];
      frame[x][y][z].types = (int)tmp[1];
      face_box(&frame[x][y][z]);
      rotateX(&frame[x][y][z], rot.x);
      rotateY(&frame[x][y][z], rot.y);
      rotateZ(&frame[x][y][z], rot.z);
    }
 }

unsigned int Make :: set_flags()
 { unsigned int result = 0;

   for(int ix = 1; ix < 65; ix++)
    for(int iy = 1; iy < 65; iy++)
     for(int iz = 1; iz < 65; iz++)
      { face_ratio(frame[ix][iy][iz].face[0], frame[ix-1][iy][iz].face[1]);
        face_ratio(frame[ix][iy][iz].face[1], frame[ix+1][iy][iz].face[0]);
        face_ratio(frame[ix][iy][iz].face[2], frame[ix][iy][iz-1].face[3]);
        face_ratio(frame[ix][iy][iz].face[3], frame[ix][iy][iz+1].face[2]);
        face_ratio(frame[ix][iy][iz].face[4], frame[ix][iy-1][iz].face[5]);
        face_ratio(frame[ix][iy][iz].face[5], frame[ix][iy+1][iz].face[4]);

        for(int i = 0; i < 6; i++)
         if(frame[ix][iy][iz].face[i] == 1) result += 2;
         else if(frame[ix][iy][iz].face[i] > 1) result += 1;
        result += faceTab[frame[ix][iy][iz].types - 1][6];
      }
   return result;
 }

void Make :: build(Mesh* mesh, unsigned int sz)
 { Vertex* vrt = new Vertex[sz * 3];
   unsigned int i = 0;
   glm::vec3 pt[8];
   glm::ivec3 tr1, tr2;
   glm::ivec4 fc;
   int n = 0;

   for(int ix = 1; ix < 65; ix++)
    for(int iy = 1; iy < 65; iy++)
     for(int iz = 1; iz < 65; iz++)
      { frame1(ix, iy, iz, pt, 0.1f);
        fc = glm::ivec4(4,0,2,6);
        n = triangles(frame[ix][iy][iz].face[0], fc, tr1, tr2);
        if(n > 0)
         { vrt[i].pos = pt[tr1.x]; i++;
           vrt[i].pos = pt[tr1.y]; i++;
           vrt[i].pos = pt[tr1.z]; i++;
         }
        if(n > 1)
         { vrt[i].pos = pt[tr2.x]; i++;
           vrt[i].pos = pt[tr2.y]; i++;
           vrt[i].pos = pt[tr2.z]; i++;
         }
        fc = glm::ivec4(7,3,1,5);
        n = triangles(frame[ix][iy][iz].face[1], fc, tr1, tr2);
        if(n > 0)
         { vrt[i].pos = pt[tr1.x]; i++;
           vrt[i].pos = pt[tr1.y]; i++;
           vrt[i].pos = pt[tr1.z]; i++;
         }
        if(n > 1)
         { vrt[i].pos = pt[tr2.x]; i++;
           vrt[i].pos = pt[tr2.y]; i++;
           vrt[i].pos = pt[tr2.z]; i++;
         }
        fc = glm::ivec4(5,1,0,4);
        n = triangles(frame[ix][iy][iz].face[2], fc, tr1, tr2);
        if(n > 0)
         { vrt[i].pos = pt[tr1.x]; i++;
           vrt[i].pos = pt[tr1.y]; i++;
           vrt[i].pos = pt[tr1.z]; i++;
         }
        if(n > 1)
         { vrt[i].pos = pt[tr2.x]; i++;
           vrt[i].pos = pt[tr2.y]; i++;
           vrt[i].pos = pt[tr2.z]; i++;
         }
        fc = glm::ivec4(6,2,3,7);
        n = triangles(frame[ix][iy][iz].face[3], fc, tr1, tr2);
        if(n > 0)
         { vrt[i].pos = pt[tr1.x]; i++;
           vrt[i].pos = pt[tr1.y]; i++;
           vrt[i].pos = pt[tr1.z]; i++;
         }
        if(n > 1)
         { vrt[i].pos = pt[tr2.x]; i++;
           vrt[i].pos = pt[tr2.y]; i++;
           vrt[i].pos = pt[tr2.z]; i++;
         }
        fc = glm::ivec4(0,1,3,2);
        n = triangles(frame[ix][iy][iz].face[4], fc, tr1, tr2);
        if(n > 0)
         { vrt[i].pos = pt[tr1.x]; i++;
           vrt[i].pos = pt[tr1.y]; i++;
           vrt[i].pos = pt[tr1.z]; i++;
         }
        if(n > 1)
         { vrt[i].pos = pt[tr2.x]; i++;
           vrt[i].pos = pt[tr2.y]; i++;
           vrt[i].pos = pt[tr2.z]; i++;
         }
        fc = glm::ivec4(4,6,7,5);
        n = triangles(frame[ix][iy][iz].face[5], fc, tr1, tr2);
        if(n > 0)
         { vrt[i].pos = pt[tr1.x]; i++;
           vrt[i].pos = pt[tr1.y]; i++;
           vrt[i].pos = pt[tr1.z]; i++;
         }
        if(n > 1)
         { vrt[i].pos = pt[tr2.x]; i++;
           vrt[i].pos = pt[tr2.y]; i++;
           vrt[i].pos = pt[tr2.z]; i++;
         }

        if(frame[ix][iy][iz].types == 2)
         { vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
         }
        if(frame[ix][iy][iz].types == 3)
         { vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;
         }
        if(frame[ix][iy][iz].types == 4)
         { vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
         }
        if(frame[ix][iy][iz].types == 5)
         { vrt[i].pos = pt[frame[ix][iy][iz].point[6]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
         }
        if(frame[ix][iy][iz].types == 6)
         { vrt[i].pos = pt[frame[ix][iy][iz].point[6]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;
         }
      }

   mesh->setVertex(vrt, i);
 }

void Make :: frame1(int x, int y, int z, glm::vec3* points, float scale)
 { glm::vec3 tmp(x * scale, y * scale, z * scale);

   points[0] = tmp;
   points[1] = glm::vec3(tmp.x + scale, tmp.y, tmp.z);
   points[2] = glm::vec3(tmp.x, tmp.y, tmp.z + scale);
   points[3] = glm::vec3(tmp.x + scale, tmp.y, tmp.z + scale);

   points[4] = glm::vec3(tmp.x, tmp.y + scale, tmp.z);
   points[5] = glm::vec3(tmp.x + scale, tmp.y + scale, tmp.z);
   points[6] = glm::vec3(tmp.x, tmp.y + scale, tmp.z + scale);
   points[7] = glm::vec3(tmp.x + scale, tmp.y + scale, tmp.z + scale);
 }

void Make :: face_box(Cell* cell)
 { for(int i = 0; i < 6; i++)
    cell->face[i] = faceTab[cell->types - 1][i];
 }

void Make :: rotateX(Cell* cell, int n)
 { char tmp;
   char box[8];

   for(char i = 0; i < 8; i++)
    box[i] = i;

   for(int i = 0; i < n; i++)
    { tmp =  box[2];
      box[2] = box[0];
      box[0] = box[4];
      box[4] = box[6];
      box[6] = tmp;
      tmp =  box[3];
      box[3] = box[1];
      box[1] = box[5];
      box[5] = box[7];
      box[7] = tmp;

      tmp = cell->face[2];

      if(cell->face[5] > 1)
       { cell->face[2] = cell->face[5] + 2;
         if(cell->face[2] > 5) cell->face[2] -= 4;
       }
      else cell->face[2] = cell->face[5];

      cell->face[5] = cell->face[3];

      if(cell->face[4] > 1)
       { cell->face[3] = cell->face[4] - 1;
         if(cell->face[3] < 2) cell->face[3] = 5;
       }
      else cell->face[3] = cell->face[4];

      if(tmp > 1)
       { cell->face[4] = tmp - 1;
         if(cell->face[4] < 2) cell->face[4] = 5;
       }
      else cell->face[4] = tmp;

      if(cell->face[1] > 1)
       { cell->face[1] += 1;
         if(cell->face[1] > 5)
          cell->face[1] = 2;
       }
      if(cell->face[0] > 1)
       { cell->face[0] -= 1;
         if(cell->face[0] < 2)
          cell->face[0] = 5;
       }
    }

   for(char i = 0; i < 8; i++)
    cell->point[box[i]] = i;
 }

void Make :: rotateY(Cell* cell, int n)
 { char tmp;
   char box[8];

   for(char i = 0; i < 8; i++)
    box[i] = cell->point[i];

   for(int i = 0; i < n; i++)
    { tmp =  box[2];
      box[2] = box[3];
      box[3] = box[1];
      box[1] = box[0];
      box[0] = tmp;
      tmp =  box[6];
      box[6] = box[7];
      box[7] = box[5];
      box[5] = box[4];
      box[4] = tmp;

      tmp = cell->face[0];

      cell->face[0] = cell->face[2];
      cell->face[2] = cell->face[1];
      cell->face[1] = cell->face[3];
      cell->face[3] = tmp;

      if(cell->face[5] > 1)
       { cell->face[5] -= 1;
         if(cell->face[5] < 2)
          cell->face[5] = 5;
       }
      if(cell->face[4] > 1)
       { cell->face[4] += 1;
         if(cell->face[4] > 5)
          cell->face[4] = 2;
       }
    }

   for(char i = 0; i < 8; i++)
    cell->point[box[i]] = i;
 }

void Make :: rotateZ(Cell* cell, int n)
 { char tmp;
   char box[8];

   for(char i = 0; i < 8; i++)
    box[i] = cell->point[i];

   for(int i = 0; i < n; i++)
    { tmp =  box[2];
      box[2] = box[3];
      box[3] = box[7];
      box[7] = box[6];
      box[6] = tmp;
      tmp =  box[0];
      box[0] = box[1];
      box[1] = box[5];
      box[5] = box[4];
      box[4] = tmp;

      tmp = cell->face[0];

      cell->face[0] = cell->face[4];

      if(cell->face[1] > 1)
       { cell->face[4] = cell->face[1] + 2;
         if(cell->face[4] > 5) cell->face[4] -= 4;
       }
      else cell->face[4] = cell->face[1];

      if(cell->face[5] > 1)
       { cell->face[1] = cell->face[5] + 1;
         if(cell->face[1] > 5) cell->face[1] = 2;
       }
      else cell->face[1] = cell->face[5];

      if(tmp > 1)
       { cell->face[5] = tmp + 1;
         if(cell->face[5] > 5) cell->face[5] = 2;
       }
      else cell->face[5] = tmp;

      if(cell->face[3] > 1)
       { cell->face[3] += 1;
         if(cell->face[3] > 5)
          cell->face[3] = 2;
       }
      if(cell->face[2] > 1)
       { cell->face[2] -= 1;
         if(cell->face[2] < 2)
          cell->face[2] = 5;
       }
    }

   for(char i = 0; i < 8; i++)
    cell->point[box[i]] = i;

 }

void Make :: face_ratio(char &f1, char &f2)
 { if(f1 == 1)
    { if(f2 == 1) { f1 = 0; f2 = 0; return; }
      if(f2 == 2) { f1 = 3; f2 = 0; return; }
      if(f2 == 3) { f1 = 2; f2 = 0; return; }
      if(f2 == 4) { f1 = 5; f2 = 0; return; }
      if(f2 == 5) { f1 = 4; f2 = 0; return; }
    }
   if(f1 == 2)
    { if(f2 == 1) { f1 = 0; f2 = 3; return; }
      if(f2 == 5) { f1 = 0; f2 = 0; return; }
    }
   if(f1 == 3)
    { if(f2 == 1) { f1 = 0; f2 = 2; return; }
      if(f2 == 4) { f1 = 0; f2 = 0; return; }
    }
   if(f1 == 4)
    { if(f2 == 1) { f1 = 0; f2 = 5; return; }
      if(f2 == 3) { f1 = 0; f2 = 0; return; }
    }
   if(f1 == 5)
    { if(f2 == 1) { f1 = 0; f2 = 4; return; }
      if(f2 == 2) { f1 = 0; f2 = 0; return; }
    }
 }

int Make :: triangles(char face, glm::ivec4 &pt, glm::ivec3& tr1, glm::ivec3 &tr2)
 { if(face == 1)
    { tr1.x = pt.x;
      tr1.y = pt.y;
      tr1.z = pt.z;
      tr2.x = pt.z;
      tr2.y = pt.w;
      tr2.z = pt.x;
      return 2;
    }
   if(face == 2)
    { tr1.x = pt.x;
      tr1.y = pt.y;
      tr1.z = pt.z;
      return 1;
    }
   if(face == 3)
    { tr1.x = pt.w;
      tr1.y = pt.x;
      tr1.z = pt.y;
      return 1;
    }
   if(face == 4)
    { tr1.x = pt.z;
      tr1.y = pt.w;
      tr1.z = pt.x;
      return 1;
    }
   if(face == 5)
    { tr1.x = pt.y;
      tr1.y = pt.z;
      tr1.z = pt.w;
      return 1;
    }
   return 0;
 }
