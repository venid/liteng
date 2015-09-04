#pragma once

#include "plane.h"
#include <glm/glm.hpp>

class Frustrum
 { private:
    Plane planes[10];
     int num_planes;

   public:
    Frustrum()
     {num_planes = 0;}

    void set(const glm::vec3& org, const glm::vec3* v, int num);
    void addPlane(Plane& plane)
     { planes[num_planes] = plane;
       num_planes++;
     }

    bool contains ( const glm::vec3& v ) const
     { for(register int i = 0; i < num_planes; i++ )
        if(planes[i].classify(v) == -1) return false;
       return true;
     }

    bool contains (glm::vec3 *v, int num) const
     { int n;
       for(register int i = 0; i < num_planes; i++)
        { n = 0;
          for(register int j = 0; j < num; j++)
           if(planes[i].classify(v[j]) == -1) n++;
          if(n == num) return false;
        }
       return true;
     }

   bool contains_sphere (const glm::vec3 &pos, const float rad) const
    { for(register int i = 0; i < num_planes; i++ )
       if ( planes[i].distance(pos) < -rad ) return false;
      return true;
    }
 };
