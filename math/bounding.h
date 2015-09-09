
#pragma once

#include <glm/glm.hpp>

class Plane;
class Frustrum;
class OBBox;
class BSphere;

class AABBox
 { private:
    glm::vec3 m_pos;
    glm::vec3 m_scale;
    glm::vec3 m_min;
    glm::vec3 m_max;
   public:
    AABBox();
    AABBox(const glm::vec3 &pos, const glm::vec3 &scale);
    AABBox(const AABBox &box);

    void setPos(const glm::vec3 &pos);
    void setScale(const glm::vec3 &scale);

    const glm::vec3& getScale() const { return m_scale; }
    const glm::vec3& getPos() const { return m_pos; }

    bool contains(const glm::vec3 &p) const
     { return p.x >= m_min.x && p.x <= m_max.x &&
              p.y >= m_min.y && p.y <= m_max.y &&
              p.z >= m_min.z && p.z <= m_max.z;
     }

    int classify(const Plane& plane) const;

    bool intersects(const BSphere& sph) const;
    bool intersects(const AABBox& box) const;
    bool intersects(const OBBox& box) const;
    bool intersects(const Frustrum& frustrum) const;

    friend class OBBox;
 };

/// ------------------------------------------------------------------- 

class OBBox
 { private:
    glm::vec3 m_scale;
    glm::vec3 m_pos;
    glm::mat4 m_trans;
   public:
    OBBox();
    OBBox(const glm::vec3& pos, const glm::vec3& scale);
    OBBox(const OBBox &box);
    OBBox(const AABBox &box);

    void setPos(const glm::vec3 &pos);
    void setScale(const glm::vec3 &scale) {m_scale = scale;}
    void setTransform(const glm::mat4 &matrix);
    void move(const glm::mat4 &matrix) { m_trans *= matrix; }

    const glm::vec3& getScale() const { return m_scale; }
    const glm::vec3& getPos() const { return m_pos; }
    const glm::mat4& getTransform() const { return m_trans; }

    bool contains(const glm::vec3 &p) const;

    int classify(const Plane& plane) const;

    bool intersects(const Frustrum& frustrum) const;
    bool intersects(const BSphere& sph) const;
    bool intersects(const AABBox& box) const;
    bool intersects(const OBBox& box) const;
 };

/// ------------------------------------------------------------------

class BSphere
 { private:
    glm::vec3 m_pos;
    float m_rad;
   public:
    BSphere();
    BSphere(const glm::vec3 &pos, float r);
    BSphere(const BSphere &sph);

    const glm::vec3& getPos() const {return m_pos;}
    const float getRadius() const {return m_rad;}

    void setPos(const glm::vec3& pos) { m_pos = pos;}
    void setRadius(const float r) {m_rad = r; }

    bool contains(const glm::vec3& p) const
     { glm::vec3 tmp(p - m_pos);
       return (tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z) <=
              (m_rad * m_rad);
     }

    int classify(const Plane& plane) const;

    bool intersects(const BSphere& bs) const;
    bool intersects(const AABBox& box) const;
    bool intersects(const OBBox& box) const;
    bool intersects(const Frustrum &frustrum) const;

    friend class OBBox;
 };
