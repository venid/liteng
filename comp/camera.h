
#pragma once
#include "component.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "luastate.h"
#include "frustrum.h"


namespace Camera
{

class Render : public Component
 { private:
    glm::ivec2 rect;

    glm::mat4 m_proj;
    glm::mat4 m_view;
    glm::mat4 m_pvMatrix;

    glm::mat4 *mp_transMatrix;

    glm::vec3 m_pos;
    glm::vec3 *mp_pos;

    Frustrum  m_frustrum;

    float m_zfar;
    float m_znear;
    float m_fov;

    float m_fovX;
    float m_fovY;

    void buildFrustrum(glm::mat4& mt);

   public:
    Render(Lua::Var& tab, unsigned int pt);
    ~Render();

    bool init();

    void linkVar(int def, void* data);

    void doUpdate();

    Frustrum& getFrustrum() { return m_frustrum; }
    glm::mat4& getPVMat() { return m_pvMatrix; }

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Render(*tab, m_p);}

    static int privat_tab[];
    static int public_tab[];

    static Meta::Base Instance;
 };

// -------------------------------------------------

class Translate : public Component
 { private:
    float m_orbitDist;
    float *mp_dist;

    glm::mat4 *mp_transMatrix;
    glm::vec2 *mp_turn;
    //glm::vec3 *m_eye;

    glm::mat4 m_matrix;
    glm::quat m_orientation;
    glm::vec3 m_pos;
    glm::vec3 m_target;

    glm::vec3 *mp_pos;

    void updateTransMatrix();
    void rotateOrbit(float headingDegrees, float pitchDegrees);
    void rotate(float headingDegrees, float pitchDegrees, float rollDegrees);
   public:
    Translate(unsigned int pt);
     ~Translate();

     void linkVar(int def, void* data);

     void doUpdate();

     static Object* Create(Lua::Var *tab, unsigned int m_p)
      {return new Translate(m_p);}

     static int privat_tab[];

     static Meta::Base Instance;
 };

// -------------------------------------------------

class Control : public Component
 { private:
    Lua::State *lvm;

    float *mp_dist;
    glm::vec2 *mp_turn;
    int m_flag;
    glm::ivec2 m_cursor;

   public:
    Control(unsigned int pt);
    ~Control();

    void linkVar(int def, void* data);
    bool init();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     { return new Control(m_p); }

    void dirDist(float volume);
    void rotation(int x, int y);
    void setFlag(int flag)
     { m_flag = flag; }
    void setCursor(int x, int y)
     { m_cursor = glm::ivec2(x, y); }

    static int privat_tab[];
    static int public_tab[];

    static Meta::Base Instance;
 };

// ------------------------------------------------

class Build : public Component
 { private:

   public:

    static Meta::Base Instance;
 };

}
