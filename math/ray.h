
#pragma once

#include <glm/glm.hpp>

class Ray
 { private:
    glm::vec3 m_org;
    glm::vec3 m_dir;

   public:
    Ray() : m_org(), m_dir()
     {}

    Ray(const glm::vec3& org, const glm::vec3& dir) : m_org(org), m_dir(dir)
     { m_dir = glm::normalize(m_dir); }

    Ray(const Ray& ray) : m_org(ray.m_org), m_dir(ray.m_dir)
     {}

    const glm::vec3& getOrigin() const  { return m_org; }
    const glm::vec3& getDir() const     { return m_dir; }

    void setOrigin(const glm::vec3 &org) { m_org = org; }
    void setDir(const glm::vec3 &dir) { m_dir = dir; }

    glm::vec3 point(float t) const      // point on ray
     { return m_org + t * m_dir; }

 };
