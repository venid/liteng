
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
 { public:
    glm::mat3 m;
    glm::vec3 v;

    Transform() : m(1.f), v(0.f)
     { }
    Transform(const glm::mat3 &matrix, const glm::vec3 &vector) : m(matrix),
                                                                  v(vector)
     { }
    Transform(const Transform& tr) : m(tr.m), v(tr.v)
     { }
    Transform(const glm::mat3& matrix) : m(matrix), v(0.f)
     { }
    Transform(const glm::mat4& matrix) : m(matrix)
     { v.x = matrix[3][0];
       v.y = matrix[3][1];
       v.z = matrix[3][2];
     }
    Transform(const glm::quat& qt) : v(0.f)
     { m = glm::mat3_cast(qt); }

    glm::vec3 transPoint(const glm::vec3& p) const
     { return glm::vec3( v + m * p); }

    glm::vec3 transDir(const glm::vec3& p) const
     { return glm::vec3(m * p); }

    void buildMat4(glm::mat4& matrix) const
     { glm::mat4 tmp(m);
       matrix = glm::translate(tmp, v);
     }

 };

inline Transform operator * (const Transform& t1, const Transform& t2)
 { return Transform(t1.m * t2.m, t1.m * t2.v + t1.v); }
