
#include "noise.h"
#include "tinymt32.h"


// -------------------------------------------------------------------------------------
#define B SAMPLE_SIZE
#define BM (SAMPLE_SIZE-1)

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )
#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.0f;

Noise::Perlin :: Perlin(int seed)
 { m_octaves = 1;
   m_frequency = 1.f;
   m_amplitude = 1.f;
   init(seed);
 }

void Noise::Perlin :: init(int seed)
 { int i, j, k;
   tinymt32_t tinymt;
   glm::vec2 v2;
   glm::vec3 v3;

   tinymt.mat1 = 0x8f7011ee;
   tinymt.mat2 = 0xfc78ff1f;
   tinymt.tmat = 0x3793fdff;
   tinymt32_init(&tinymt, seed);
	  for (i = 0 ; i < SAMPLE_SIZE ; i++)
    { p[i] = i;
		    g1[i] = (float)((tinymt32_generate_uint32(&tinymt) % (B + B)) - B) / B;
		    v2.x = (float)((tinymt32_generate_uint32(&tinymt) % (B + B)) - B) / B;
      v2.y = (float)((tinymt32_generate_uint32(&tinymt) % (B + B)) - B) / B;
		    v2 = glm::normalize(v2);
      g2[i][0] = v2.x;
      g2[i][1] = v2.y;
		    v3.x = (float)((tinymt32_generate_uint32(&tinymt) % (B + B)) - B) / B;
      v3.y = (float)((tinymt32_generate_uint32(&tinymt) % (B + B)) - B) / B;
      v3.z = (float)((tinymt32_generate_uint32(&tinymt) % (B + B)) - B) / B;
		    v3 = glm::normalize(v3);
      g3[i][0] = v3.x;
      g3[i][1] = v3.y;
      g3[i][2] = v3.z;
	   }

	   while (--i)
     { k = p[i];
		     p[i] = p[j = tinymt32_generate_uint32(&tinymt) % B];
		     p[j] = k;
	    }

	   for (i = 0 ; i < B + 2 ; i++)
     { p[B + i] = p[i];
		     g1[B + i] = g1[i];
		     for (j = 0 ; j < 2 ; j++)
			     g2[B + i][j] = g2[i][j];
		     for (j = 0 ; j < 3 ; j++)
			     g3[B + i][j] = g3[i][j];
	    }
         
 }

float Noise::Perlin :: noise(float &vec)
 { float result = 0.f;
   

   return result;
 }

float Noise::Perlin :: noise(glm::vec2 &vec)
 { float result = 0.f;
   

   return result;
 }

float Noise::Perlin :: noise(glm::vec3 &vec)
 { int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	  float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	  int i, j;

	  setup(0, bx0,bx1, rx0,rx1);
	  setup(1, by0,by1, ry0,ry1);
	  setup(2, bz0,bz1, rz0,rz1);

	  i = p[ bx0 ];
	  j = p[ bx1 ];

	  b00 = p[ i + by0 ];
	  b10 = p[ j + by0 ];
	  b01 = p[ i + by1 ];
	  b11 = p[ j + by1 ];

	  t  = s_curve(rx0);
	  sy = s_curve(ry0);
	  sz = s_curve(rz0);

   #define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	  q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	  q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	  a = lerp(t, u, v);

	  q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	  q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	  b = lerp(t, u, v);

	  c = lerp(sy, a, b);

	  q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	  q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	  a = lerp(t, u, v);

	  q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	  q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	  b = lerp(t, u, v);

	  d = lerp(sy, a, b);

	  return lerp(sz, c, d);
 }

// -------------------------------------------------------------------------------------


