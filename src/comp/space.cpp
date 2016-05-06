
#include "space.h"
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
#include "noise.h"
#include <glm/gtx/normal.hpp>

using namespace Space;

META_METHODS(Body,
 METHOD(create, Body::Create))
META_PROPERTY(Body)
META_OBJECT(Body, Space::Body, &Component::Instance)

int Body :: privat_tab[] = {vVECTOR_SEGMENT, 0};
int Body :: public_tab[] = {vRES_MANAGER, 0};

// -------------------------------------------------------------------------------

#define _SIZE_ 100
const char SZ_CH = _SIZE_ + 2;
const char SZ_V = _SIZE_ + 3;

#define SMU 0.2f

int chunk[SZ_CH][SZ_CH][SZ_CH];
float density[SZ_CH][SZ_CH][SZ_CH];
glm::vec3 norm[SZ_CH][SZ_CH][SZ_CH][6];
struct Vert
 { glm::vec3 pt;
   bool valid;
 } vert[SZ_V][SZ_V][SZ_V], vert2[SZ_V][SZ_V][SZ_V];

// ------------------------------------------------------------------------------

Body :: Body(Lua::Var &tab, unsigned int pt) : Component(pt)
 { m_fun = (std::string)tab["fun"];

   m_update = (CUpdate) &Body::doUpdate;
   privat_var = Body::privat_tab;
   public_var = Body::public_tab;
   m_id = 0;
   metaClass = &Instance;
 }

void Body :: linkVar(int def, void* data)
 { switch(def)
    { case vVECTOR_SEGMENT:
       mp_segment = (std::vector<Segment*>*)data;
       break;
      case vRES_MANAGER:
       mp_res = (*(ResManager**)data);
       break;
    }
 }

bool Body :: init()
 { Shape* shp;
   Segment* sgm;
   Mesh* mesh;
   glm::vec3 ps(0.f);
   Timer tmr;

   Data *data = mp_res->getResource(m_fun);
   mp_res->lvm.load_data(data);

   mesh = new Mesh(m_fun.c_str());

   tmr.start();
   primary_form(ps);
   noise(ps);
   unsigned int n = set_flags();
   create_point();
   create_normal();
   build(mesh, n, 0.1f);
   tmr.stop();
   n *= 2;
   LOG_INFO("Space::Body: количество полигонов = %i", n);
   LOG_INFO("Space::Body: время генерации = %f", tmr.getSampleTime());
   mp_res->regObject(mesh);

   m_unit->lock();
   sgm = new Segment();
   shp = new Shape("test_01");
   shp->setMaterial((Material*)mp_res->getObject("mt01.mat"));
   shp->setMesh((Mesh*)mp_res->getObject(m_fun.c_str()));
   sgm->addShape(shp);
   m_unit->unlock();

   mp_segment->push_back(sgm);
   return true;
 }

void Body :: doUpdate()
 { glm::vec3 ps(0.f);
   Timer tmr;
   Mesh* mesh = (Mesh*)mp_res->getObject(m_fun.c_str());

   Data *data = mp_res->getResource(m_fun);
   mp_res->lvm.load_data(data);

   tmr.start();
   primary_form(ps);
   noise(ps);
   unsigned int n = set_flags();
   create_point();
   create_normal();
   build(mesh, n, 0.1f);
   tmr.stop();
   n *= 2;
   LOG_INFO("Space::Body: количество полигонов = %i", n);
   LOG_INFO("Space::Body: время генерации = %f", tmr.getSampleTime());

   mesh->release();
 }

// ----------------------------------------------------------------------------------

void mildNorm(glm::vec3& res, glm::vec3& a, glm::vec3& b, float angl)
 { if((b != glm::vec3(0.f))&&(glm::dot(a, b) > angl)) res += b; }

void Body :: primary_form(glm::vec3 &offset)
 { glm::vec3 tmp;
   div_t divresult = div(_SIZE_, 2);
   int hsz = divresult.quot + 1;
   float dn;
   float scale = 0.1f;
   Lua::Var gfun = mp_res->lvm["body"];

   memset(chunk, 0, sizeof(chunk));

   for(int ix = 1; ix < SZ_CH-1; ix++)
   for(int iy = 1; iy < SZ_CH-1; iy++)
    for(int iz = 1; iz < SZ_CH-1; iz++)
     { tmp = glm::vec3((ix-hsz) * scale + offset.x,
                       (iy-hsz) * scale + offset.y,
                       (iz-hsz) * scale + offset.z);
       dn = gfun(tmp.x, tmp.y, tmp.z);

       if(dn < 1.f)
        { chunk[ix][iy][iz] = BOX_13;
          density[ix][iy][iz] = 1.f - dn;
        }
       else
        { chunk[ix][iy][iz] = 0;
          density[ix][iy][iz] = 0.f;
        }
     }
 }

void Body :: noise(glm::vec3 &offset)
 { float scale = 0.1f;
   glm::vec3 bias;
   div_t divresult = div(_SIZE_, 2);
   int hsz = divresult.quot + 1;
   Lua::Var tab = mp_res->lvm["noise"];
   if(tab != Lua::TAB) return;
   int rnd = tab["rnd"];
   int flags = tab["symmetry"];
   int num_process = tab.size();
   Noise::Perlin *perlin = new Noise::Perlin(rnd);

   for (int i = 0; i < num_process; i++)
    { Lua::Var process = tab[i+1];
      int oct = process["octaves"];
      perlin->setOctaves(oct);
      float freq = process["frequency"];
      float amp = process["amplitude"];
      perlin->setWave(freq, amp);
      float press = process["pressure"];

      for(int ix = 1; ix < SZ_CH-1; ix++)
       for(int iy = 1; iy < SZ_CH-1; iy++)
        for(int iz = 1; iz < SZ_CH-1; iz++)
         { if((chunk[ix][iy][iz] & BOX_13) == 0) continue;
           bias = glm::vec3((ix-hsz) * scale, (iy-hsz) * scale, (iz-hsz) * scale);
           if((flags & 0x00000001) == 0x00000001) bias.x = fabs(bias.x);
           if((flags & 0x00000002) == 0x00000002) bias.y = fabs(bias.y);
           if((flags & 0x00000004) == 0x00000004) bias.z = fabs(bias.z);
           density[ix][iy][iz] -= perlin->get(bias) * press;
           if(density[ix][iy][iz] < 0.f) chunk[ix][iy][iz] = 0;
         }
    }
   delete perlin;
 }

unsigned int Body :: set_flags()
 { unsigned int result = 0;
   unsigned int tmp;
   unsigned int n = 0;
   unsigned int nn = 0;
   unsigned int nnn = 0;

   for(int ix = 1; ix < SZ_CH-1; ix++)
   for(int iy = 1; iy < SZ_CH-1; iy++)
    for(int iz = 1; iz < SZ_CH-1; iz++)
     if((chunk[ix][iy][iz] & BOX_13) == BOX_13)
      { tmp = 0;
        if((chunk[ix-1][iy-1][iz-1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_00;
        if((chunk[ix][iy-1][iz-1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_01;
        if((chunk[ix+1][iy-1][iz-1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_02;
        if((chunk[ix-1][iy][iz-1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_03;
        if((chunk[ix][iy][iz-1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_04; else tmp++;
        if((chunk[ix+1][iy][iz-1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_05;
        if((chunk[ix-1][iy+1][iz-1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_06;
        if((chunk[ix][iy+1][iz-1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_07;
        if((chunk[ix+1][iy+1][iz-1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_08;

        if((chunk[ix-1][iy-1][iz] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_09;
        if((chunk[ix][iy-1][iz] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_10; else tmp++;
        if((chunk[ix+1][iy-1][iz] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_11;
        if((chunk[ix-1][iy][iz] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_12; else tmp++;
        if((chunk[ix+1][iy][iz] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_14; else tmp++;
        if((chunk[ix-1][iy+1][iz] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_15;
        if((chunk[ix][iy+1][iz] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_16; else tmp++;
        if((chunk[ix+1][iy+1][iz] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_17;

        if((chunk[ix-1][iy-1][iz+1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_18;
        if((chunk[ix][iy-1][iz+1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_19;
        if((chunk[ix+1][iy-1][iz+1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_20;
        if((chunk[ix-1][iy][iz+1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_21;
        if((chunk[ix][iy][iz+1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_22; else tmp++;
        if((chunk[ix+1][iy][iz+1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_23;
        if((chunk[ix-1][iy+1][iz+1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_24;
        if((chunk[ix][iy+1][iz+1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_25;
        if((chunk[ix+1][iy+1][iz+1] & BOX_13) == BOX_13) chunk[ix][iy][iz] |= BOX_26;

       nnn++;
       if(tmp == 0) { chunk[ix][iy][iz] |= BOX_FF; nn++; }
       if(tmp == 6) { chunk[ix][iy][iz] = 0; tmp = 0; n++; nnn--; }
       result += tmp;
      }
   LOG_SPAM("Space::Body: общие количество блоков %i", nnn);
   LOG_SPAM("Space::Body: количество одиночных блоков %i", n);
   LOG_SPAM("Space::Body: количество скрытых блоков %i", nn);
   return result;
 }

void Body :: create_point()
 { int n;
   float sc = ((float)SZ_V * 0.5f) * 0.1f;
   glm::vec3 tmp;

   for(int ix = 0; ix < SZ_V; ix++)
    for(int iy = 0; iy < SZ_V; iy++)
     for(int iz = 0; iz < SZ_V; iz++)
      { if((ix == 0)||(ix == SZ_V-1)||
           (iy == 0)||(iy == SZ_V-1)||
           (iz == 0)||(iz == SZ_V-1)) { vert2[ix][iy][iz].valid = false; continue; }
        n = 0;
        if((chunk[ix][iy][iz] & BOX_13) == 0) n++;
        if((chunk[ix-1][iy][iz] & BOX_13) == 0) n++;
        if((chunk[ix][iy][iz-1] & BOX_13) == 0) n++;
        if((chunk[ix-1][iy][iz-1] & BOX_13) == 0) n++;
        if((chunk[ix][iy-1][iz] & BOX_13) == 0) n++;
        if((chunk[ix-1][iy-1][iz] & BOX_13) == 0) n++;
        if((chunk[ix][iy-1][iz-1] & BOX_13) == 0) n++;
        if((chunk[ix-1][iy-1][iz-1] & BOX_13) == 0) n++;
        if(n == 8) { vert2[ix][iy][iz].valid = false; continue; }
        if(n == 0) { vert2[ix][iy][iz].valid = false; continue; }

        vert2[ix][iy][iz].valid = true;
        vert2[ix][iy][iz].pt = glm::vec3((float)ix*0.1f-sc, (float)iy*0.1f-sc, (float)iz*0.1f-sc);
      }
   memcpy(vert, vert2, sizeof(vert));
   for(int r = 0; r < 3; r++)
   {
    for(int ix = 1; ix < SZ_V-1; ix++)
    for(int iy = 1; iy < SZ_V-1; iy++)
     for(int iz = 1; iz < SZ_V-1; iz++)
      { if(vert2[ix][iy][iz].valid == false) continue;
        n = 1;
        tmp = vert2[ix][iy][iz].pt;
        if(vert2[ix-1][iy][iz].valid) { tmp += vert2[ix-1][iy][iz].pt; n++; }
        if(vert2[ix+1][iy][iz].valid) { tmp += vert2[ix+1][iy][iz].pt; n++; }

        if(vert2[ix][iy-1][iz].valid) { tmp += vert2[ix][iy-1][iz].pt; n++; }
        if(vert2[ix][iy+1][iz].valid) { tmp += vert2[ix][iy+1][iz].pt; n++; }

        if(vert2[ix][iy][iz-1].valid) { tmp += vert2[ix][iy][iz-1].pt; n++; }
        if(vert2[ix][iy][iz+1].valid) { tmp += vert2[ix][iy][iz+1].pt; n++; }

        /*if(vert2[ix-1][iy+1][iz-1].valid) { tmp += vert2[ix-1][iy+1][iz-1].pt; n++; }
        if(vert2[ix+1][iy-1][iz+1].valid) { tmp += vert2[ix+1][iy-1][iz+1].pt; n++; }

        if(vert2[ix-1][iy-1][iz-1].valid) { tmp += vert2[ix-1][iy-1][iz-1].pt; n++; }
        if(vert2[ix+1][iy+1][iz+1].valid) { tmp += vert2[ix+1][iy+1][iz+1].pt; n++; }

        if(vert2[ix-1][iy+1][iz+1].valid) { tmp += vert2[ix-1][iy+1][iz+1].pt; n++; }
        if(vert2[ix+1][iy-1][iz-1].valid) { tmp += vert2[ix+1][iy-1][iz-1].pt; n++; }

        if(vert2[ix-1][iy-1][iz+1].valid) { tmp += vert2[ix-1][iy-1][iz+1].pt; n++; }
        if(vert2[ix+1][iy+1][iz-1].valid) { tmp += vert2[ix+1][iy+1][iz-1].pt; n++; }*/

        if(vert2[ix-1][iy-1][iz].valid) { tmp += vert2[ix-1][iy-1][iz].pt; n++; }
        if(vert2[ix+1][iy+1][iz].valid) { tmp += vert2[ix+1][iy+1][iz].pt; n++; }

        if(vert2[ix-1][iy+1][iz].valid) { tmp += vert2[ix-1][iy+1][iz].pt; n++; }
        if(vert2[ix+1][iy-1][iz].valid) { tmp += vert2[ix+1][iy-1][iz].pt; n++; }

        if(vert2[ix][iy+1][iz+1].valid) { tmp += vert2[ix][iy+1][iz+1].pt; n++; }
        if(vert2[ix][iy-1][iz-1].valid) { tmp += vert2[ix][iy-1][iz-1].pt; n++; }

        if(vert2[ix][iy+1][iz-1].valid) { tmp += vert2[ix][iy+1][iz-1].pt; n++; }
        if(vert2[ix][iy-1][iz+1].valid) { tmp += vert2[ix][iy-1][iz+1].pt; n++; }

        if(vert2[ix-1][iy][iz-1].valid) { tmp += vert2[ix-1][iy][iz-1].pt; n++; }
        if(vert2[ix+1][iy][iz+1].valid) { tmp += vert2[ix+1][iy][iz+1].pt; n++; }

        if(vert2[ix-1][iy][iz+1].valid) { tmp += vert2[ix-1][iy][iz+1].pt; n++; }
        if(vert2[ix+1][iy][iz-1].valid) { tmp += vert2[ix+1][iy][iz-1].pt; n++; }

        vert[ix][iy][iz].pt = tmp / (float)n;
      }
     memcpy(vert2, vert, sizeof(vert));
   }
 }

void Body :: create_normal()
 { glm::vec3 pt[4];
   glm::vec3 tmp;

   memset(norm, 0, sizeof(norm));

   for(int ix = 1; ix < SZ_CH-1; ix++)
    for(int iy = 1; iy < SZ_CH-1; iy++)
     for(int iz = 1; iz < SZ_CH-1; iz++)
      { if(((chunk[ix][iy][iz] & BOX_13) == 0)||((chunk[ix][iy][iz] & BOX_FF) == BOX_FF)) continue;

        if((chunk[ix][iy][iz] & BOX_16) == 0)
         { pt[0] = vert[ix][iy+1][iz].pt;
           pt[1] = vert[ix][iy+1][iz+1].pt;
           pt[2] = vert[ix+1][iy+1][iz+1].pt;
           pt[3] = vert[ix+1][iy+1][iz].pt;
           norm[ix][iy][iz][0] = (glm::triangleNormal(pt[1], pt[2], pt[0]) + 
                                  glm::triangleNormal(pt[3], pt[0], pt[2])) / 2.f;
         }
        if((chunk[ix][iy][iz] & BOX_10) == 0)
         { pt[0] = vert[ix][iy][iz].pt;
           pt[1] = vert[ix+1][iy][iz].pt;
           pt[2] = vert[ix+1][iy][iz+1].pt;
           pt[3] = vert[ix][iy][iz+1].pt;
           norm[ix][iy][iz][1] = (glm::triangleNormal(pt[1], pt[2], pt[0]) +
                                  glm::triangleNormal(pt[3], pt[0], pt[2])) / 2.f;
         }
        if((chunk[ix][iy][iz] & BOX_14) == 0)
         { pt[0] = vert[ix+1][iy+1][iz+1].pt;
           pt[1] = vert[ix+1][iy][iz+1].pt;
           pt[2] = vert[ix+1][iy][iz].pt;
           pt[3] = vert[ix+1][iy+1][iz].pt;
           norm[ix][iy][iz][2] = (glm::triangleNormal(pt[1], pt[2], pt[0]) +
                                  glm::triangleNormal(pt[3], pt[0], pt[2])) / 2.f;
         }
        if((chunk[ix][iy][iz] & BOX_12) == 0)
         { pt[0] = vert[ix][iy+1][iz+1].pt;
           pt[1] = vert[ix][iy+1][iz].pt;
           pt[2] = vert[ix][iy][iz].pt;
           pt[3] = vert[ix][iy][iz+1].pt;
           norm[ix][iy][iz][3] = (glm::triangleNormal(pt[1], pt[2], pt[0]) +
                                  glm::triangleNormal(pt[3], pt[0], pt[2])) / 2.f;
         }
        if((chunk[ix][iy][iz] & BOX_22) == 0)
         { pt[0] = vert[ix+1][iy+1][iz+1].pt;
           pt[1] = vert[ix][iy+1][iz+1].pt;
           pt[2] = vert[ix][iy][iz+1].pt;
           pt[3] = vert[ix+1][iy][iz+1].pt;
           norm[ix][iy][iz][4] = (glm::triangleNormal(pt[1], pt[2], pt[0]) +
                                  glm::triangleNormal(pt[3], pt[0], pt[2])) / 2.f;
         }
        if((chunk[ix][iy][iz] & BOX_04) == 0)
         { pt[0] = vert[ix+1][iy+1][iz].pt;
           pt[1] = vert[ix+1][iy][iz].pt;
           pt[2] = vert[ix][iy][iz].pt;
           pt[3] = vert[ix][iy+1][iz].pt;
           norm[ix][iy][iz][5] = (glm::triangleNormal(pt[1], pt[2], pt[0]) +
                                  glm::triangleNormal(pt[3], pt[0], pt[2])) / 2.f;
         }
      }
 }

void Body :: build(Mesh* mesh, unsigned int sz, float scale)
 { Vertex* vrt = new Vertex[sz * 6];
   unsigned int i = 0;
   glm::vec3 nl[4];
   Vertex tmp;
   glm::vec3 vec;

   for(int ix = 1; ix < SZ_CH-1; ix++)
    for(int iy = 1; iy < SZ_CH-1; iy++)
     for(int iz = 1; iz < SZ_CH-1; iz++)
      { if(((chunk[ix][iy][iz] & BOX_13) == 0)||((chunk[ix][iy][iz] & BOX_FF) == BOX_FF)) continue;

        if((chunk[ix][iy][iz] & BOX_16) == 0)
         { nl[0] = norm[ix][iy][iz][0];
           nl[1] = norm[ix][iy][iz][0];
           nl[2] = norm[ix][iy][iz][0];
           nl[3] = norm[ix][iy][iz][0];

           mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix][iy][iz][5], SMU);
           mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix][iy][iz][3], SMU);
           if((chunk[ix][iy][iz] & BOX_12) == BOX_12)
            { mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix-1][iy][iz][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix-1][iy][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_04) == BOX_04)
            { mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix][iy][iz-1][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix][iy][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_03) == BOX_03)
            { mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix-1][iy][iz-1][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix-1][iy][iz-1][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix-1][iy][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_15) == BOX_15)
            { mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix-1][iy+1][iz][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix-1][iy+1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_07) == BOX_07)
            { mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix][iy+1][iz-1][4], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix][iy+1][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_06) == BOX_06)
            { mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix-1][iy+1][iz-1][4], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][0], norm[ix-1][iy+1][iz-1][2], SMU);
            }

           mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix][iy][iz][4], SMU);
           mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix][iy][iz][3], SMU);
           if((chunk[ix][iy][iz] & BOX_12) == BOX_12)
            { mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix-1][iy][iz][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix-1][iy][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_22) == BOX_22)
            { mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix][iy][iz+1][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix][iy][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_21) == BOX_21)
            { mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix-1][iy][iz+1][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix-1][iy][iz+1][5], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix-1][iy][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_15) == BOX_15)
            { mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix-1][iy+1][iz][2], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix-1][iy+1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_25) == BOX_25)
            { mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix][iy+1][iz+1][5], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix][iy+1][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_24) == BOX_24)
            { mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix-1][iy+1][iz+1][5], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][0], norm[ix-1][iy+1][iz+1][2], SMU);
            }

           mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix][iy][iz][4], SMU);
           mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix][iy][iz][2], SMU);
           if((chunk[ix][iy][iz] & BOX_14) == BOX_14)
            { mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix+1][iy][iz][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix+1][iy][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_22) == BOX_22)
            { mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix][iy][iz+1][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix][iy][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_23) == BOX_23)
            { mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix+1][iy][iz+1][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix+1][iy][iz+1][5], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix+1][iy][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_17) == BOX_17)
            { mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix+1][iy+1][iz][3], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix+1][iy+1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_25) == BOX_25)
            { mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix][iy+1][iz+1][5], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix][iy+1][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_26) == BOX_26)
            { mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix+1][iy+1][iz+1][5], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][0], norm[ix+1][iy+1][iz+1][3], SMU);
            }

           mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix][iy][iz][2], SMU);
           mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix][iy][iz][5], SMU);
           if((chunk[ix][iy][iz] & BOX_14) == BOX_14)
            { mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix+1][iy][iz][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix+1][iy][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_04) == BOX_04)
            { mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix][iy][iz-1][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix][iy][iz-1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_05) == BOX_05)
            { mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix+1][iy][iz-1][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix+1][iy][iz-1][4], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix+1][iy][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_17) == BOX_17)
            { mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix+1][iy+1][iz][3], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix+1][iy+1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_07) == BOX_07)
            { mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix][iy+1][iz-1][4], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix][iy+1][iz-1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_08) == BOX_08)
            { mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix+1][iy+1][iz-1][4], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][0], norm[ix+1][iy+1][iz-1][3], SMU);
            }


           nl[0] = glm::normalize(nl[0]);
           nl[1] = glm::normalize(nl[1]);
           nl[2] = glm::normalize(nl[2]);
           nl[3] = glm::normalize(nl[3]);

           vrt[i].pos = vert[ix][iy+1][iz].pt;     vrt[i].norm = nl[0]; i++; 
           vrt[i].pos = vert[ix][iy+1][iz+1].pt;   vrt[i].norm = nl[1]; i++;
           vrt[i].pos = vert[ix+1][iy+1][iz+1].pt; vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix+1][iy+1][iz+1].pt; vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix+1][iy+1][iz].pt;   vrt[i].norm = nl[3]; i++;
           vrt[i].pos = vert[ix][iy+1][iz].pt;     vrt[i].norm = nl[0]; i++;
         }

        if((chunk[ix][iy][iz] & BOX_10) == 0)
         { nl[0] = norm[ix][iy][iz][1];
           nl[1] = norm[ix][iy][iz][1];
           nl[2] = norm[ix][iy][iz][1];
           nl[3] = norm[ix][iy][iz][1];

           mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix][iy][iz][5], SMU);
           mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix][iy][iz][3], SMU);
           if((chunk[ix][iy][iz] & BOX_12) == BOX_12)
            { mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix-1][iy][iz][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix-1][iy][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_04) == BOX_04)
            { mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix][iy][iz-1][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix][iy][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_03) == BOX_03)
            { mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix-1][iy][iz-1][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix-1][iy][iz-1][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix-1][iy][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_09) == BOX_09)
            { mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix-1][iy-1][iz][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix-1][iy-1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_01) == BOX_07)
            { mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix][iy-1][iz-1][4], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix][iy-1][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_00) == BOX_00)
            { mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix-1][iy-1][iz-1][4], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][1], norm[ix-1][iy-1][iz-1][2], SMU);
            }

           mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix][iy][iz][2], SMU);
           mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix][iy][iz][5], SMU);
           if((chunk[ix][iy][iz] & BOX_14) == BOX_14)
            { mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix+1][iy][iz][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix+1][iy][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_04) == BOX_04)
            { mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix][iy][iz-1][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix][iy][iz-1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_05) == BOX_05)
            { mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix+1][iy][iz-1][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix+1][iy][iz-1][4], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix+1][iy][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_11) == BOX_11)
            { mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix+1][iy-1][iz][3], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix+1][iy-1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_01) == BOX_01)
            { mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix][iy-1][iz-1][4], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix][iy-1][iz-1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_02) == BOX_02)
            { mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix+1][iy-1][iz-1][4], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][1], norm[ix+1][iy-1][iz-1][3], SMU);
            }

           mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix][iy][iz][4], SMU);
           mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix][iy][iz][2], SMU);
           if((chunk[ix][iy][iz] & BOX_14) == BOX_14)
            { mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix+1][iy][iz][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix+1][iy][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_22) == BOX_22)
            { mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix][iy][iz+1][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix][iy][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_23) == BOX_23)
            { mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix+1][iy][iz+1][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix+1][iy][iz+1][5], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix+1][iy][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_11) == BOX_11)
            { mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix+1][iy-1][iz][3], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix+1][iy-1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_19) == BOX_19)
            { mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix][iy-1][iz+1][5], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix][iy-1][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_20) == BOX_20)
            { mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix+1][iy-1][iz+1][5], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][1], norm[ix+1][iy-1][iz+1][3], SMU);
            } 

           mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix][iy][iz][4], SMU);
           mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix][iy][iz][3], SMU);
           if((chunk[ix][iy][iz] & BOX_12) == BOX_12)
            { mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix-1][iy][iz][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix-1][iy][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_22) == BOX_22)
            { mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix][iy][iz+1][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix][iy][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_21) == BOX_21)
            { mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix-1][iy][iz+1][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix-1][iy][iz+1][5], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix-1][iy][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_09) == BOX_09)
            { mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix-1][iy-1][iz][2], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix-1][iy-1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_19) == BOX_19)
            { mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix][iy-1][iz+1][5], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix][iy-1][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_18) == BOX_18)
            { mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix-1][iy-1][iz+1][5], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][1], norm[ix-1][iy-1][iz+1][2], SMU);
            } 

           nl[0] = glm::normalize(nl[0]);
           nl[1] = glm::normalize(nl[1]);
           nl[2] = glm::normalize(nl[2]);
           nl[3] = glm::normalize(nl[3]);

           vrt[i].pos = vert[ix][iy][iz].pt;     vrt[i].norm = nl[0]; i++; 
           vrt[i].pos = vert[ix+1][iy][iz].pt;   vrt[i].norm = nl[1]; i++;
           vrt[i].pos = vert[ix+1][iy][iz+1].pt; vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix+1][iy][iz+1].pt; vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix][iy][iz+1].pt;   vrt[i].norm = nl[3]; i++;
           vrt[i].pos = vert[ix][iy][iz].pt;     vrt[i].norm = nl[0]; i++;
         }

        if((chunk[ix][iy][iz] & BOX_14) == 0)
         { nl[0] = norm[ix][iy][iz][2];
           nl[1] = norm[ix][iy][iz][2];
           nl[2] = norm[ix][iy][iz][2];
           nl[3] = norm[ix][iy][iz][2];

           mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix][iy][iz][0], SMU);
           mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix][iy][iz][4], SMU);
           if((chunk[ix][iy][iz] & BOX_16) == BOX_16)
            { mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix][iy+1][iz][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix][iy+1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_22) == BOX_22)
            { mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix][iy][iz+1][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix][iy][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_25) == BOX_25)
            { mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix][iy+1][iz+1][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix][iy+1][iz+1][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix][iy+1][iz+1][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_17) == BOX_17)
            { mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix+1][iy+1][iz][4], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix+1][iy+1][iz][1], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_23) == BOX_23)
            { mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix+1][iy][iz+1][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix+1][iy][iz+1][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_26) == BOX_26)
            { mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix+1][iy+1][iz+1][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][2], norm[ix+1][iy+1][iz+1][5], SMU);
            }

           mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix][iy][iz][1], SMU);
           mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix][iy][iz][4], SMU);
           if((chunk[ix][iy][iz] & BOX_10) == BOX_10)
            { mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix][iy-1][iz][2], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix][iy-1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_22) == BOX_22)
            { mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix][iy][iz+1][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix][iy][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_19) == BOX_19)
            { mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix][iy-1][iz+1][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix][iy-1][iz+1][2], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix][iy-1][iz+1][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_11) == BOX_11)
            { mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix+1][iy-1][iz][4], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix+1][iy-1][iz][0], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_23) == BOX_23)
            { mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix+1][iy][iz+1][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix+1][iy][iz+1][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_20) == BOX_20)
            { mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix+1][iy-1][iz+1][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][2], norm[ix+1][iy-1][iz+1][5], SMU);
            }

           mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix][iy][iz][1], SMU);
           mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix][iy][iz][5], SMU);
           if((chunk[ix][iy][iz] & BOX_10) == BOX_10)
            { mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix][iy-1][iz][2], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix][iy-1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_04) == BOX_04)
            { mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix][iy][iz-1][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix][iy][iz-1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_01) == BOX_01)
            { mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix][iy-1][iz-1][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix][iy-1][iz-1][2], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix][iy-1][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_11) == BOX_11)
            { mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix+1][iy-1][iz][5], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix+1][iy-1][iz][0], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_05) == BOX_05)
            { mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix+1][iy][iz-1][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix+1][iy][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_02) == BOX_02)
            { mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix+1][iy-1][iz-1][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][2], norm[ix+1][iy-1][iz-1][4], SMU);
            }

           mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix][iy][iz][0], SMU);
           mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix][iy][iz][5], SMU);
           if((chunk[ix][iy][iz] & BOX_16) == BOX_16)
            { mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix][iy+1][iz][2], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix][iy+1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_04) == BOX_04)
            { mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix][iy][iz-1][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix][iy][iz-1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_07) == BOX_07)
            { mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix][iy+1][iz-1][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix][iy+1][iz-1][2], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix][iy+1][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_17) == BOX_17)
            { mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix+1][iy+1][iz][5], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix+1][iy+1][iz][1], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_05) == BOX_05)
            { mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix+1][iy][iz-1][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix+1][iy][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_08) == BOX_08)
            { mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix+1][iy+1][iz-1][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][2], norm[ix+1][iy+1][iz-1][4], SMU);
            } 

           nl[0] = glm::normalize(nl[0]);
           nl[1] = glm::normalize(nl[1]);
           nl[2] = glm::normalize(nl[2]);
           nl[3] = glm::normalize(nl[3]);

           vrt[i].pos = vert[ix+1][iy+1][iz+1].pt; vrt[i].norm = nl[0]; i++; 
           vrt[i].pos = vert[ix+1][iy][iz+1].pt;   vrt[i].norm = nl[1]; i++;
           vrt[i].pos = vert[ix+1][iy][iz].pt;     vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix+1][iy][iz].pt;     vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix+1][iy+1][iz].pt;   vrt[i].norm = nl[3]; i++;
           vrt[i].pos = vert[ix+1][iy+1][iz+1].pt; vrt[i].norm = nl[0]; i++;
         }

        if((chunk[ix][iy][iz] & BOX_12) == 0)
         { nl[0] = norm[ix][iy][iz][3];
           nl[1] = norm[ix][iy][iz][3];
           nl[2] = norm[ix][iy][iz][3];
           nl[3] = norm[ix][iy][iz][3];

           mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix][iy][iz][0], SMU);
           mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix][iy][iz][4], SMU);
           if((chunk[ix][iy][iz] & BOX_16) == BOX_16)
            { mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix][iy+1][iz][3], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix][iy+1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_22) == BOX_22)
            { mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix][iy][iz+1][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix][iy][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_25) == BOX_25)
            { mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix][iy+1][iz+1][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix][iy+1][iz+1][3], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix][iy+1][iz+1][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_15) == BOX_15)
            { mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix-1][iy+1][iz][4], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix-1][iy+1][iz][1], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_21) == BOX_21)
            { mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix-1][iy][iz+1][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix-1][iy][iz+1][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_24) == BOX_24)
            { mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix-1][iy+1][iz+1][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][3], norm[ix-1][iy+1][iz+1][5], SMU);
            }

           mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix][iy][iz][0], SMU);
           mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix][iy][iz][5], SMU);
           if((chunk[ix][iy][iz] & BOX_16) == BOX_16)
            { mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix][iy+1][iz][3], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix][iy+1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_04) == BOX_04)
            { mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix][iy][iz-1][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix][iy][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_07) == BOX_07)
            { mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix][iy+1][iz-1][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix][iy+1][iz-1][3], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix][iy+1][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_15) == BOX_15)
            { mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix-1][iy+1][iz][5], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix-1][iy+1][iz][1], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_03) == BOX_03)
            { mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix-1][iy][iz-1][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix-1][iy][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_06) == BOX_06)
            { mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix-1][iy+1][iz-1][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][3], norm[ix-1][iy+1][iz-1][4], SMU); 
            }
           
           mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix][iy][iz][1], SMU);
           mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix][iy][iz][5], SMU);
           if((chunk[ix][iy][iz] & BOX_10) == BOX_10)
            { mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix][iy-1][iz][3], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix][iy-1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_04) == BOX_04)
            { mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix][iy][iz-1][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix][iy][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_01) == BOX_01)
            { mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix][iy-1][iz-1][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix][iy-1][iz-1][3], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix][iy-1][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_09) == BOX_09)
            { mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix-1][iy-1][iz][5], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix-1][iy-1][iz][0], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_03) == BOX_03)
            { mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix-1][iy][iz-1][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix-1][iy][iz-1][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_02) == BOX_02)
            { mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix-1][iy-1][iz-1][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][3], norm[ix-1][iy-1][iz-1][4], SMU);
            }

           mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix][iy][iz][1], SMU);
           mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix][iy][iz][4], SMU);
           if((chunk[ix][iy][iz] & BOX_10) == BOX_10)
            { mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix][iy-1][iz][3], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix][iy-1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_22) == BOX_22)
            { mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix][iy][iz+1][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix][iy][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_19) == BOX_19)
            { mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix][iy-1][iz+1][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix][iy-1][iz+1][3], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix][iy-1][iz+1][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_09) == BOX_09)
            { mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix-1][iy-1][iz][4], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix-1][iy-1][iz][0], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_21) == BOX_21)
            { mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix-1][iy][iz+1][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix-1][iy][iz+1][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_18) == BOX_18)
            { mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix-1][iy-1][iz+1][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][3], norm[ix-1][iy-1][iz+1][5], SMU);
            } 

           nl[0] = glm::normalize(nl[0]);
           nl[1] = glm::normalize(nl[1]);
           nl[2] = glm::normalize(nl[2]);
           nl[3] = glm::normalize(nl[3]);

           vrt[i].pos = vert[ix][iy+1][iz+1].pt; vrt[i].norm = nl[0]; i++; 
           vrt[i].pos = vert[ix][iy+1][iz].pt;   vrt[i].norm = nl[1]; i++;
           vrt[i].pos = vert[ix][iy][iz].pt;     vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix][iy][iz].pt;     vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix][iy][iz+1].pt;   vrt[i].norm = nl[3]; i++;
           vrt[i].pos = vert[ix][iy+1][iz+1].pt; vrt[i].norm = nl[0]; i++;
         }

        if((chunk[ix][iy][iz] & BOX_22) == 0)
         { nl[0] = norm[ix][iy][iz][4];
           nl[1] = norm[ix][iy][iz][4];
           nl[2] = norm[ix][iy][iz][4];
           nl[3] = norm[ix][iy][iz][4];

           mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix][iy][iz][0], SMU);
           mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix][iy][iz][2], SMU);
           if((chunk[ix][iy][iz] & BOX_16) == BOX_16)
            { mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix][iy+1][iz][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix][iy+1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_14) == BOX_14)
            { mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix+1][iy][iz][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix+1][iy][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_17) == BOX_17)
            { mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix+1][iy+1][iz][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix+1][iy+1][iz][3], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix+1][iy+1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_25) == BOX_25)
            { mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix][iy+1][iz+1][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix][iy+1][iz+1][1], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_23) == BOX_23)
            { mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix+1][iy][iz+1][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix+1][iy][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_26) == BOX_26)
            { mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix+1][iy+1][iz+1][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][4], norm[ix+1][iy+1][iz+1][3], SMU);
            }

           mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix][iy][iz][0], SMU);
           mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix][iy][iz][3], SMU);
           if((chunk[ix][iy][iz] & BOX_16) == BOX_16)
            { mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix][iy+1][iz][3], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix][iy+1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_12) == BOX_12)
            { mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix-1][iy][iz][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix-1][iy][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_15) == BOX_15)
            { mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix-1][iy+1][iz][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix-1][iy+1][iz][2], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix-1][iy+1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_25) == BOX_25)
            { mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix][iy+1][iz+1][3], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix][iy+1][iz+1][1], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_21) == BOX_21)
            { mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix-1][iy][iz+1][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix-1][iy][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_24) == BOX_24)
            { mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix-1][iy+1][iz+1][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][4], norm[ix-1][iy+1][iz+1][2], SMU);
            }

           mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix][iy][iz][1], SMU);
           mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix][iy][iz][3], SMU);
           if((chunk[ix][iy][iz] & BOX_10) == BOX_10)
            { mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix][iy-1][iz][3], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix][iy-1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_12) == BOX_12)
            { mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix-1][iy][iz][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix-1][iy][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_09) == BOX_09)
            { mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix-1][iy-1][iz][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix-1][iy-1][iz][2], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix-1][iy-1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_19) == BOX_19)
            { mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix][iy-1][iz+1][3], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix][iy-1][iz+1][0], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_21) == BOX_21)
            { mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix-1][iy][iz+1][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix-1][iy][iz+1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_18) == BOX_18)
            { mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix-1][iy-1][iz+1][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][4], norm[ix-1][iy-1][iz+1][2], SMU);
            }

           mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix][iy][iz][1], SMU);
           mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix][iy][iz][2], SMU);
           if((chunk[ix][iy][iz] & BOX_10) == BOX_10)
            { mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix][iy-1][iz][2], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix][iy-1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_14) == BOX_14)
            { mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix+1][iy][iz][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix+1][iy][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_11) == BOX_11)
            { mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix+1][iy-1][iz][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix+1][iy-1][iz][3], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix+1][iy-1][iz][4], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_19) == BOX_19)
            { mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix][iy-1][iz+1][2], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix][iy-1][iz+1][0], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_23) == BOX_23)
            { mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix+1][iy][iz+1][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix+1][iy][iz+1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_20) == BOX_20)
            { mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix+1][iy-1][iz+1][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][4], norm[ix+1][iy-1][iz+1][3], SMU);
            }

           nl[0] = glm::normalize(nl[0]);
           nl[1] = glm::normalize(nl[1]);
           nl[2] = glm::normalize(nl[2]);
           nl[3] = glm::normalize(nl[3]);

           vrt[i].pos = vert[ix+1][iy+1][iz+1].pt; vrt[i].norm = nl[0]; i++; 
           vrt[i].pos = vert[ix][iy+1][iz+1].pt;   vrt[i].norm = nl[1]; i++;
           vrt[i].pos = vert[ix][iy][iz+1].pt;     vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix][iy][iz+1].pt;     vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix+1][iy][iz+1].pt;   vrt[i].norm = nl[3]; i++;
           vrt[i].pos = vert[ix+1][iy+1][iz+1].pt; vrt[i].norm = nl[0]; i++;
         }

        if((chunk[ix][iy][iz] & BOX_04) == 0)
         { nl[0] = norm[ix][iy][iz][5];
           nl[1] = norm[ix][iy][iz][5];
           nl[2] = norm[ix][iy][iz][5];
           nl[3] = norm[ix][iy][iz][5];

           mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix][iy][iz][0], SMU);
           mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix][iy][iz][2], SMU);
           if((chunk[ix][iy][iz] & BOX_16) == BOX_16)
            { mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix][iy+1][iz][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix][iy+1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_14) == BOX_14)
            { mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix+1][iy][iz][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix+1][iy][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_17) == BOX_17)
            { mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix+1][iy+1][iz][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix+1][iy+1][iz][3], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix+1][iy+1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_07) == BOX_07)
            { mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix][iy+1][iz-1][2], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix][iy+1][iz-1][1], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_05) == BOX_05)
            { mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix+1][iy][iz-1][0], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix+1][iy][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_08) == BOX_08)
            { mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix+1][iy+1][iz-1][1], SMU);
              mildNorm(nl[0], norm[ix][iy][iz][5], norm[ix+1][iy+1][iz-1][3], SMU);
            } 

           mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix][iy][iz][1], SMU);
           mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix][iy][iz][2], SMU);
           if((chunk[ix][iy][iz] & BOX_10) == BOX_10)
            { mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix][iy-1][iz][2], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix][iy-1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_14) == BOX_14)
            { mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix+1][iy][iz][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix+1][iy][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_11) == BOX_11)
            { mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix+1][iy-1][iz][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix+1][iy-1][iz][3], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix+1][iy-1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_01) == BOX_01)
            { mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix][iy-1][iz-1][2], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix][iy-1][iz-1][0], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_05) == BOX_05)
            { mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix+1][iy][iz-1][1], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix+1][iy][iz-1][3], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_02) == BOX_02)
            { mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix+1][iy-1][iz-1][0], SMU);
              mildNorm(nl[1], norm[ix][iy][iz][5], norm[ix+1][iy-1][iz-1][3], SMU);
            }

           mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix][iy][iz][1], SMU);
           mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix][iy][iz][3], SMU);
           if((chunk[ix][iy][iz] & BOX_10) == BOX_10)
            { mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix][iy-1][iz][3], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix][iy-1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_12) == BOX_12)
            { mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix-1][iy][iz][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix-1][iy][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_09) == BOX_09)
            { mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix-1][iy-1][iz][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix-1][iy-1][iz][2], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix-1][iy-1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_01) == BOX_01)
            { mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix][iy-1][iz-1][3], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix][iy-1][iz-1][0], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_03) == BOX_03)
            { mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix-1][iy][iz-1][1], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix-1][iy][iz-1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_00) == BOX_00)
            { mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix-1][iy-1][iz-1][0], SMU);
              mildNorm(nl[2], norm[ix][iy][iz][5], norm[ix-1][iy-1][iz-1][2], SMU);
            }

           mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix][iy][iz][0], SMU);
           mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix][iy][iz][3], SMU);
           if((chunk[ix][iy][iz] & BOX_16) == BOX_16)
            { mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix][iy+1][iz][3], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix][iy+1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_12) == BOX_12)
            { mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix-1][iy][iz][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix-1][iy][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_15) == BOX_15)
            { mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix-1][iy+1][iz][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix-1][iy+1][iz][2], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix-1][iy+1][iz][5], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_07) == BOX_07)
            { mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix][iy+1][iz-1][3], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix][iy+1][iz-1][1], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_03) == BOX_03)
            { mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix-1][iy][iz-1][0], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix-1][iy][iz-1][2], SMU);
            }
           if((chunk[ix][iy][iz] & BOX_06) == BOX_06)
            { mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix-1][iy+1][iz-1][1], SMU);
              mildNorm(nl[3], norm[ix][iy][iz][5], norm[ix-1][iy+1][iz-1][2], SMU);
            }

           nl[0] = glm::normalize(nl[0]);
           nl[1] = glm::normalize(nl[1]);
           nl[2] = glm::normalize(nl[2]);
           nl[3] = glm::normalize(nl[3]);

           vrt[i].pos = vert[ix+1][iy+1][iz].pt; vrt[i].norm = nl[0]; i++; 
           vrt[i].pos = vert[ix+1][iy][iz].pt;   vrt[i].norm = nl[1]; i++;
           vrt[i].pos = vert[ix][iy][iz].pt;     vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix][iy][iz].pt;     vrt[i].norm = nl[2]; i++;
           vrt[i].pos = vert[ix][iy+1][iz].pt;   vrt[i].norm = nl[3]; i++;
           vrt[i].pos = vert[ix+1][iy+1][iz].pt; vrt[i].norm = nl[0]; i++;
         }
       }
   mesh->lock();
   mesh->setVertex(vrt, i);
   mesh->unlock();
 }
