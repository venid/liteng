
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
   mt[3][0] = 1.7f;
   mt[3][1] = 1.15f;
   mt[3][2] = -0.7f;
   memcpy(mp_trans, &mt, sizeof(glm::mat4));

   return true;
 }

void Make :: doUpdate()
 { }

// ------------------------------------------------------------------------------

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
      rotate(&frame[x][y][z], rot);
      frame[x][y][z].types = (int)tmp[1];
      face_box(&frame[x][y][z]);
    }
 }

unsigned int Make :: set_flags()
 { unsigned int result = 0;
   
   for(int ix = 1; ix < 65; ix++)
   for(int iy = 1; iy < 65; iy++)
    for(int iz = 1; iz < 65; iz++)
     switch(frame[ix][iy][iz].types)
      { case 1:
         if((frame[ix][iy][iz].face[0] == 1) && (frame[ix-1][iy][iz].face[1] == 1))
          { frame[ix][iy][iz].face[0] = 0;
            frame[ix-1][iy][iz].face[1] = 0;
          }
         if((frame[ix][iy][iz].face[1] == 1) && (frame[ix+1][iy][iz].face[0] == 1))
          { frame[ix][iy][iz].face[1] = 0;
            frame[ix+1][iy][iz].face[0] = 0;
          }
         if((frame[ix][iy][iz].face[2] == 1) && (frame[ix][iy][iz-1].face[3] == 1))
          { frame[ix][iy][iz].face[2] = 0;
            frame[ix][iy][iz-1].face[3] = 0;
          }
         if((frame[ix][iy][iz].face[3]) && (frame[ix][iy][iz+1].face[2] == 1))
          { frame[ix][iy][iz].face[3] = 0;
            frame[ix][iy][iz+1].face[2] = 0;
          }
         if((frame[ix][iy][iz].face[4]) && (frame[ix][iy-1][iz].face[5] == 1))
          { frame[ix][iy][iz].face[4] = 0;
            frame[ix][iy-1][iz].face[5] = 0;
          }
         if((frame[ix][iy][iz].face[5]) && (frame[ix][iy+1][iz].face[4] == 1))
          { frame[ix][iy][iz].face[5] = 0;
            frame[ix][iy+1][iz].face[4] = 0;
          }

         for(int i = 0; i < 6; i++)
          if(frame[ix][iy][iz].face[i] == 1) result += 2;
         break;
        case 2: result += 8;  break;
      }
    return result;
 }

void Make :: build(Mesh* mesh, unsigned int sz)
 { Vertex* vrt = new Vertex[sz * 3];
   unsigned int i = 0;
   glm::vec3 pt[8];
   glm::vec3 tmp;
   glm::vec3 dir(1.f);
   glm::mat3 rot(1.f);

   for(int ix = 1; ix < 65; ix++)
    for(int iy = 1; iy < 65; iy++)
     for(int iz = 1; iz < 65; iz++)
      { if(frame[ix][iy][iz].types == 2)
         { frame1(ix, iy, iz, pt, 0.1f);

           vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;

           vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;

           vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;

           vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;

           vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
           vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
         }

        if(frame[ix][iy][iz].types == 1)
         { frame1(ix, iy, iz, pt, 0.1f);

           if(frame[ix][iy][iz].face[0] == 1)
            { vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[6]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
            }

           if(frame[ix][iy][iz].face[1] == 1)
            { vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[7]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[7]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;
            }

           if(frame[ix][iy][iz].face[2] == 1)
            { vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
            }

           if(frame[ix][iy][iz].face[3] == 1)
            { vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[7]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[6]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[6]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
            }

           if(frame[ix][iy][iz].face[4] == 1)
            { vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[1]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[3]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[2]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[0]]; i++;
            }

           if(frame[ix][iy][iz].face[5] == 1)
            { vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[6]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[7]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[7]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[5]]; i++;
              vrt[i].pos = pt[frame[ix][iy][iz].point[4]]; i++;
            }
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
 { switch(cell->types)
    { case 1:
       cell->face[0] = 1;
       cell->face[1] = 1;
       cell->face[2] = 1;
       cell->face[3] = 1;
       cell->face[4] = 1;
       cell->face[5] = 1;
       break;
      case 2:
       cell->face[0] = 0;
       cell->face[1] = 0;
       cell->face[2] = 1;
       cell->face[3] = 0;
       cell->face[4] = 1;
       cell->face[5] = 0;
       break;
    }
 }

void Make :: rotate(Cell* cell, glm::ivec3 &rot)
 { char tmp;
   char box[8];

   for(char i = 0; i < 8; i++)
    box[i] = i;

   for(int i = 0; i < rot.x; i++)
    { tmp =  box[2];
      box[2] = box[6];
      box[6] = box[4];
      box[4] = box[0];
      box[0] = tmp;
      tmp =  box[3];
      box[3] = box[7];
      box[7] = box[5];
      box[5] = box[1];
      box[1] = tmp;

      tmp = cell->face[2];
      cell->face[2] = cell->face[4];
      cell->face[4] = cell->face[3];
      cell->face[3] = cell->face[5];
      cell->face[5] = tmp;
    }

   for(char i = 0; i < 8; i++)
    cell->point[box[i]] = i;
 }
