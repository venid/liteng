#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/constants.hpp>
#include "ray.h"

class Plane
 { public:
    glm::vec3 n;
    float dist;

    Plane()
     { n = glm::vec3(0.f, 1.f, 0.f);
      dist = 0.f;
     }

    Plane(const glm::vec3 &norm, float ds)
     { n = glm::normalize(norm);
       dist = ds;
     }

    Plane(float nx, float ny, float nz, float d)
     { n = glm::normalize(glm::vec3(nx, ny, nz));
       dist = d;
     }

    Plane(const glm::vec3 &norm, const glm::vec3 &point)
     { n = glm::normalize(norm);
       dist = -glm::dot(point, n);
     }

    Plane(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
     { n = glm::normalize(glm::triangleNormal(p1, p2, p3));
       dist = -glm::dot(p1, n);
     }

    Plane(const Plane& plane) : n(plane.n), dist(plane.dist) {}


    Plane& operator = ( const Plane& pl )
     { n = pl.n;
       dist = pl.dist;
       return *this;
     }

    bool operator == (const Plane& pl)
     { return (n == pl.n) && (dist == pl.dist); }

    Plane& operator *= (const float f)
     { n = glm::normalize(n * f);
       dist *= f;
       return *this;
     }

    void inverse()
     { n *= -1.f; }

    Plane inverted() const
     { Plane res = *this;
       res.inverse();
       return res;
     }

    float dot(const glm::vec4& v) const
     { return n.x*v.x + n.y*v.y + n.z*v.z + dist*v.w ; }

    float dotCoord (const glm::vec3& v) const
     { return n.x*v.x + n.y*v.y + n.z*v.z + dist; }

    float dotNormal (const glm::vec3& v) const
     { return n.x*v.x + n.y*v.y + n.z*v.z; }

    float distance(const glm::vec3 &point) const
     { return dist + glm::dot(point, n); }

    float distance_abs(const glm::vec3 &point) const
     { return glm::abs(dist + glm::dot(point, n)); }

    glm::vec3 getPoint() const
     { return n * (-dist); }

    int classify(const glm::vec3 &point) const
     { float v = dist + glm::dot(point, n);
       float eps = glm::epsilon<float>();
       if ( v > eps ) return 1;
       else
        if ( v < -eps ) return -1;
       return 0;
     }

    void flip()
     { n = -n;
       dist = -dist;
     }

    float closest_point(const glm::vec3 &p, glm::vec3 &res ) const
     { float dst = - dist - glm::dot(p, n);
       res = p + dst * n;
       return dst;
     }

   float intersects(const Ray &ray) const
     { float numer = - (dist + glm::dot(ray.getOrigin(), n));
       float denom = glm::dot(ray.getDir(), n);
       if ( glm::abs(denom) < glm::epsilon<float>()) return 0.f;
       float t = numer / denom;
       return t > 0.f ? t : -1.f;
     }
 };
