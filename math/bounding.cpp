
#include "bounding.h"
#include "frustrum.h"
#include "plane.h"

#include "log.h"

AABBox :: AABBox() : m_pos(0.f), m_scale(0.f), m_min(0.f), m_max(0.f)
 { }

AABBox :: AABBox(const glm::vec3 &pos, const glm::vec3 &scale)
 { m_pos = pos;
   m_scale = scale;
   m_min = m_pos - m_scale;
   m_max = m_pos + m_scale;
 }

AABBox :: AABBox(const AABBox &box)
 { m_pos = box.m_pos;
   m_scale = box.m_scale;
   m_min = box.m_min;
   m_max = box.m_max;
 }

void AABBox :: setPos(const glm::vec3 &pos)
 { m_pos = pos;
   m_min = m_pos - m_scale;
   m_max = m_pos + m_scale;
 }

void AABBox :: setScale(const glm::vec3 &scale)
 { m_scale = scale;
   m_min = m_pos - m_scale;
   m_max = m_pos + m_scale;
 }

int AABBox :: classify(const Plane& plane) const
 { int n = 0;
   glm::vec3  tmp[8];

   tmp[0] = glm::vec3(m_min.x, m_min.y, m_min.z);
   tmp[1] = glm::vec3(m_min.x, m_min.y, m_max.z);
   tmp[2] = glm::vec3(m_min.x, m_max.y, m_max.z);
   tmp[3] = glm::vec3(m_min.x, m_max.y, m_min.z);
   tmp[4] = glm::vec3(m_max.x, m_max.y, m_max.z);
   tmp[5] = glm::vec3(m_max.x, m_max.y, m_min.z);
   tmp[6] = glm::vec3(m_max.x, m_min.y, m_min.z);
   tmp[7] = glm::vec3(m_max.x, m_min.y, m_max.z);

   for(int i=0; i<8; i++)
     n += plane.classify(tmp[i]);

   if(n == 8) return 1;
   if(n == -8) return -1;

   return 0;
 }

float AABBox :: intersects(const Ray& ray) const
 { glm::vec3 pos = ray.getOrigin();
   glm::vec3 dir = ray.getDir();

   if( fabs(dir.x) <= glm::epsilon<float>() &&
       fabs(dir.y) <= glm::epsilon<float>() &&
       fabs(dir.z) <= glm::epsilon<float>() )
    return -1.f;

   //Проверим если луч находится внутри параллелепипеда.
   if (pos.x >= m_min.x &&
       pos.x <= m_max.x &&
       pos.y >= m_min.y &&
       pos.y <= m_max.y &&
       pos.z >= m_min.z &&
       pos.z <= m_max.z)
    return 0.f;

   glm::vec3 tMin = (m_min - pos) / dir;
   glm::vec3 tMax = (m_max - pos) / dir;

   float minX = glm::min(tMin.x, tMax.x);
   float minY = glm::min(tMin.y, tMax.y);
   float minZ = glm::min(tMin.z, tMax.z);

   float maxX = glm::max(tMin.x, tMax.x);
   float maxY = glm::max(tMin.y, tMax.y);
   float maxZ = glm::max(tMin.z, tMax.z);

   float near = glm::max(minX, glm::max(minY, minZ));
   float far  = glm::min(maxX, glm::min(maxY, maxZ));

   if (near > far) return -1;
   if (far < 0.0f) return -1;

   if (near > 0.0f) return near;
   return far;
 }

bool AABBox :: intersects(const BSphere& sph) const
 { float d = 0, a;
   float rad = sph.getRadius();
   const glm::vec3 ps = sph.getPos();

   for (int i=0;i<3;i++)  // проходим по осям X,Y,Z
    { if (ps[i] < m_min[i])  // если центр сферы лежит перед AABB,
       { a = ps[i] - m_min[i]; // то вычисляем квадрат расстояния по этой оси
         d += a * a;
       }
      if (ps[i] > m_max[i])  // если центр сферы лежит после AABB,
       { a = ps[i] - m_max[i];  // то вычисляем квадрат расстояния по этой оси
         d += a * a;
       }
    }
   return d <= (rad * rad);
 }

bool AABBox :: intersects(const AABBox& box) const
 { const glm::vec3 T = box.m_pos - m_pos;
   return fabs(T.x) <= (m_scale.x + box.m_scale.x) &&
          fabs(T.y) <= (m_scale.y + box.m_scale.y) &&
          fabs(T.z) <= (m_scale.z + box.m_scale.z);
 }

bool AABBox :: intersects(const OBBox& box) const
 { return box.intersects(*this); }

bool AABBox :: intersects(const Frustrum& frustrum) const
 { glm::vec3 tmp[8];

   tmp[0] = glm::vec3(m_min.x, m_min.y, m_min.z);
   tmp[1] = glm::vec3(m_min.x, m_min.y, m_max.z);
   tmp[2] = glm::vec3(m_min.x, m_max.y, m_max.z);
   tmp[3] = glm::vec3(m_min.x, m_max.y, m_min.z);
   tmp[4] = glm::vec3(m_max.x, m_max.y, m_max.z);
   tmp[5] = glm::vec3(m_max.x, m_max.y, m_min.z);
   tmp[6] = glm::vec3(m_max.x, m_min.y, m_min.z);
   tmp[7] = glm::vec3(m_max.x, m_min.y, m_max.z);
   return frustrum.contains(tmp, 8);
 }

/// -------------------------------------------------------------------------------

OBBox :: OBBox() : m_scale(0.f), m_pos(0.f), m_trans(1.f)
 { }

OBBox :: OBBox(const glm::vec3& pos, const glm::vec3& scale) : m_pos(pos), m_trans(1.f)
 { m_scale = scale;
   m_trans[3][0] = pos.x;
   m_trans[3][1] = pos.y;
   m_trans[3][2] = pos.z;
 }

OBBox :: OBBox(const OBBox &box) : m_scale(box.m_scale), m_pos(box.m_pos), m_trans(box.m_trans)
 { }

OBBox :: OBBox(const AABBox &box) : m_scale(box.m_scale), m_pos(box.m_pos), m_trans(1.f)
 { m_trans[3][0] = box.m_pos.x;
   m_trans[3][1] = box.m_pos.y;
   m_trans[3][2] = box.m_pos.z;
 }

void OBBox :: setPos(const glm::vec3 &pos)
 { m_pos = pos;

   m_trans[3][0] = pos.x;
   m_trans[3][1] = pos.y;
   m_trans[3][2] = pos.z;
 }

void OBBox :: setTransform(const glm::mat4 &matrix)
 { m_trans = matrix;
   m_pos = glm::vec3(m_trans[3][0], m_trans[3][1], m_trans[3][2]);
 }

void OBBox :: move(const glm::mat4 &matrix)
 { m_trans *= matrix;
   m_pos = glm::vec3(m_trans[3][0], m_trans[3][1], m_trans[3][2]);
 }

bool OBBox :: contains(const glm::vec3 &p) const
 { glm::vec3 pt = glm::vec3(m_trans * glm::vec4(p.x, p.y, p.z, 1.f));
   AABBox box(m_pos, m_scale);
   return box.contains(pt);
 }

int OBBox :: classify(const Plane& plane) const
 { int n = 0;
   glm::vec3  tmp[8];
   glm::vec3 min = - m_scale;
   glm::vec3 max = m_scale;

   tmp[0] = glm::vec3(m_trans * glm::vec4(min.x, min.y, min.z, 1.f));
   tmp[1] = glm::vec3(m_trans * glm::vec4(min.x, min.y, max.z, 1.f));
   tmp[2] = glm::vec3(m_trans * glm::vec4(min.x, max.y, max.z, 1.f));
   tmp[3] = glm::vec3(m_trans * glm::vec4(min.x, max.y, min.z, 1.f));
   tmp[4] = glm::vec3(m_trans * glm::vec4(max.x, max.y, max.z, 1.f));
   tmp[5] = glm::vec3(m_trans * glm::vec4(max.x, max.y, min.z, 1.f));
   tmp[6] = glm::vec3(m_trans * glm::vec4(max.x, min.y, min.z, 1.f));
   tmp[7] = glm::vec3(m_trans * glm::vec4(max.x, min.y, max.z, 1.f));

   for(int i=0; i<8; i++)
     n += plane.classify(tmp[i]);

   if(n == 8) return 1;
   if(n == -8) return -1;

   return 0;
 }

bool OBBox :: intersects(const Frustrum& frustrum) const
 { glm::vec3  tmp[8];
   glm::vec3 min = - m_scale;
   glm::vec3 max = m_scale;

   tmp[0] = glm::vec3(m_trans * glm::vec4(min.x, min.y, min.z, 1.f));
   tmp[1] = glm::vec3(m_trans * glm::vec4(min.x, min.y, max.z, 1.f));
   tmp[2] = glm::vec3(m_trans * glm::vec4(min.x, max.y, max.z, 1.f));
   tmp[3] = glm::vec3(m_trans * glm::vec4(min.x, max.y, min.z, 1.f));
   tmp[4] = glm::vec3(m_trans * glm::vec4(max.x, max.y, max.z, 1.f));
   tmp[5] = glm::vec3(m_trans * glm::vec4(max.x, max.y, min.z, 1.f));
   tmp[6] = glm::vec3(m_trans * glm::vec4(max.x, min.y, min.z, 1.f));
   tmp[7] = glm::vec3(m_trans * glm::vec4(max.x, min.y, max.z, 1.f));

   return frustrum.contains(tmp, 8);
 }

bool OBBox :: intersects(const BSphere& sph) const
 { glm::vec3 min = m_pos - m_scale;
   glm::vec3 max = m_pos + m_scale;
   // трансформируем сферу в систему координат OBB
   glm::vec3 vrem = glm::vec3(m_trans * glm::vec4(sph.m_pos.x, sph.m_pos.y, sph.m_pos.z, 1.f));
   float d = 0, a ;

   // проходим по осям X,Y,Z
   for (int i=0; i<3; i++)
    { // если центр сферы лежит перед AABB,
      if (vrem[i] < min[i])
       { // то вычисляем квадрат расстояния по этой оси
         a = vrem[i] - min[i];
         d += a * a;
       }
      // если центр сферы лежит после AABB,
      if (vrem[i] > max[i])
       { // то вычисляем квадрат расстояния по этой оси
         a = vrem[i] - max[i];
         d += a * a;
       }
    }
   return d <= (sph.m_rad * sph.m_rad);
 }

bool OBBox :: intersects(const AABBox& box) const
 { OBBox tmp(box.m_pos, box.m_scale);
   return intersects(tmp);
 }

bool OBBox :: intersects(const OBBox& box) const
 { //float t, ra, rb;
   //Vector3D a = scale,
   //         b = box.scale;
//
//   Vector3D T = mat * (box.pos - pos);
//
//   Matrix3D matt = box.mat;
//   const Matrix3D R = (mat * matt.transpose());
//
//   for(int i=0; i<3 ; i++)
//    {
//      rb = b[0] * fabs(R[i][0]) + b[1] * fabs(R[i][1]) + b[2] * fabs(R[i][2]);
//      t = fabs(T[i]);
//      if ( t > a[i] + rb ) return false;
//    }
//
//   for(int k=0 ; k<3 ; k++ )
//    {
//      ra = a[0] * fabs(R[0][k]) + a[1] * fabs(R[1][k]) + a[2] * fabs(R[2][k]);
//      t = fabs(T[0] * R[0][k] + T[1] *R [1][k] + T[2] * R[2][k]);
//      if (t > ra + b[k]) return false;
//    }
//
//   //9 векторных произведений
//   //L = A0 x B0
//   ra = a[1] * fabs(R[2][0]) + a[2] * fabs(R[1][0]);
//   rb = b[1] * fabs(R[0][2]) + b[2] * fabs(R[0][1]);
//   t = fabs(T[2] * R[1][0] - T[1] * R[2][0]);
//   if (t > ra + rb) return false;
//
//   //L = A0 x B1
//   ra = a[1] * fabs(R[2][1]) + a[2] * fabs(R[1][1]);
//   rb = b[0] * fabs(R[0][2]) + b[2] * fabs(R[0][0]);
//   t = fabs(T[2] * R[1][1] - T[1] * R[2][1]);
//   if (t > ra + rb) return false;
//
//   //L = A0 x B2
//   ra = a[1] * fabs(R[2][2]) + a[2] * fabs(R[1][2]);
//   rb = b[0] * fabs(R[0][1]) + b[1] * fabs(R[0][0]);
//   t = fabs(T[2] * R[1][2] - T[1] * R[2][2]);
//   if (t > ra + rb) return false;
//
//   //L = A1 x B0
//   ra = a[0] * fabs(R[2][0]) + a[2] * fabs(R[0][0]);
//   rb = b[1] * fabs(R[1][2]) + b[2] * fabs(R[1][1]);
//   t = fabs(T[0] * R[2][0] - T[2] * R[0][0]);
//   if (t > ra + rb) return false;
//
//   //L = A1 x B1
//   ra = a[0] * fabs(R[2][1]) + a[2] * fabs(R[0][1]);
//   rb = b[0] * fabs(R[1][2]) + b[2] * fabs(R[1][0]);
//   t = fabs(T[0] * R[2][1] - T[2] * R[0][1]);
//   if (t > ra + rb) return false;
//
//   //L = A1 x B2
//   ra = a[0] * fabs(R[2][2]) + a[2] * fabs(R[0][2]);
//   rb = b[0] * fabs(R[1][1]) + b[1] * fabs(R[1][0]);
//   t = fabs(T[0] * R[2][2] - T[2] * R[0][2]);
//   if (t > ra + rb) return false;
//
//   //L = A2 x B0
//   ra = a[0] * fabs(R[1][0]) + a[1] * fabs(R[0][0]);
//   rb = b[1] * fabs(R[2][2]) + b[2] * fabs(R[2][1]);
//   t = fabs(T[1] * R[0][0] - T[0] * R[1][0]);
//   if (t > ra + rb) return false;
//
//   //L = A2 x B1
//   ra = a[0] * fabs(R[1][1]) + a[1] * fabs(R[0][1]);
//   rb = b[0] * fabs(R[2][2]) + b[2] * fabs(R[2][0]);
//   t = fabs(T[1] * R[0][1] - T[0] * R[1][1]);
//   if (t > ra + rb) return false;
//
//   //L = A2 x B2
//   ra = a[0] * fabs(R[1][2]) + a[1] * fabs(R[0][2]);
//   rb = b[0] * fabs(R[2][1]) + b[1] * fabs(R[2][0]);
//   t = fabs(T[1] * R[0][2] - T[0] * R[1][2]);
//   if (t > ra + rb) return false;
//
//   return true;
   return false;
 }

/// ---------------------------------------------------------------------------------

BSphere :: BSphere() : m_pos(0.f), m_rad(0.f)
 { }

BSphere :: BSphere(const glm::vec3 &pos, float r) : m_pos(pos), m_rad(r)
 { }

BSphere :: BSphere(const BSphere &sph)
 { m_pos = sph.m_pos;
   m_rad = sph.m_rad;
 }

int BSphere :: classify(const Plane& plane) const
 { float dist = plane.distance(m_pos);
   if(m_rad > fabs(dist)) return 0;
   if(dist < 0.f) return -1;
   return 1;
 }

float BSphere :: intersects(const Ray& ray) const
 { glm::vec3 k = ray.getOrigin() - m_pos;
   float b = glm::dot(k, ray.getDir());
   float c = glm::dot(k, k) - m_rad * m_rad;
   float d = b*b - c;

   if(d >= 0.f)
    { float sqrtfd = sqrtf(d);
      float t1 = -b + sqrtfd;
      float t2 = -b - sqrtfd;

      float min_t  = glm::min(t1,t2);
      float max_t = glm::max(t1,t2);

      if(min_t >= 0 ) return min_t;
      else if(max_t >= 0) return 0.f;
           else return -1.f;
    }
   return -1.f;
 }

bool BSphere :: intersects(const BSphere& bs) const
 { glm::vec3 tmp(m_pos - bs.m_pos);
   return (tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z) <=
          ((m_rad + bs.m_rad) * (m_rad + bs.m_rad));
 }

bool BSphere :: intersects(const AABBox& box) const
 { return box.intersects(*this); }

bool BSphere :: intersects(const OBBox& box) const
 { return box.intersects(*this); }

bool BSphere :: intersects(const Frustrum &frustrum) const
 { return frustrum.contains_sphere(m_pos, m_rad); }

