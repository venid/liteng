
#include "frustrum.h"
#include <glm/gtc/type_ptr.hpp>

void Frustrum :: set(const glm::vec3& org, const glm::vec3* v, int num)
 { glm::vec3 c(v[0]);
   int prev = num - 1;

   num_planes = 0;
   for (register int i = 1; i < num; i++ )
    c += v[i];
   c /= (float)num;

   for(int cur = 0; cur < num; cur++)
    { planes[num_planes] = Plane(org, v[prev], v[cur]);
      if (planes[num_planes].classify(c) == -1) planes[num_planes].flip();
      num_planes++;
      prev = cur;
    }
 }
