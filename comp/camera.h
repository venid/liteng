
#pragma once
#include "component.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "luastate.h"
#include "frustrum.h"


namespace Camera
{

#define CAM_FLIGHT  0x01
#define CAM_ORBIT   0x02
//#define CAM_FREE    0x04

class Render : public Component
 { private:
    float m_aspect;

    glm::mat4 m_proj;
    glm::mat4 m_ortho;

    glm::mat4 m_view;
    glm::mat4 m_pvMatrix;

    glm::mat4 *mp_transMatrix;

    float m_zfar;
    float m_znear;
    float m_fov;

    glm::vec4 *mp_dimensions;

   public:
    Render(Lua::Var& tab, unsigned int pt);
    ~Render();

    bool init();

    void linkVar(int def, void* data);

    void doUpdate();

    glm::mat4& getProject() { return m_proj; }
    glm::mat4& getView() { return m_view; }
    glm::mat4& getPVMat() { return m_pvMatrix; }
    glm::mat4& getOrthoMat() { return m_ortho; }

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Render(*tab, m_p);}

    static int privat_tab[];
    static int public_tab[];

    static Meta::Base Instance;
 };

// -------------------------------------------------

class Flight : public Component
 { private:
    int*  mp_type;

    glm::mat4* mp_view;
    glm::mat4  m_view;

    glm::vec3 *mp_turn;

    glm::quat m_orientation;

    void rotate(float heading, float pitch, float roll);
    void updateView();

   public:
    Flight(unsigned int pt);
    ~Flight();

    void linkVar(int def, void* data);

    void doUpdate();

    static Object* Create(Lua::Var *tab, unsigned int m_p)
     {return new Flight(m_p);}

    static int privat_tab[];

    static Meta::Base Instance;
 };

// -------------------------------------------------

class Orbit : public Component
 { private:
    int*  mp_type;

    glm::mat4* mp_view;
    glm::mat4  m_view;

    float* mp_dist;
    float  m_dist;

    glm::vec3 *mp_turn;

    glm::quat m_orientation;

    void rotate(float heading, float pitch);
    void updateView();

   public:
    Orbit(unsigned int pt);
    ~Orbit();

    void linkVar(int def, void* data);

    void doUpdate();

    static Object* Create(Lua::Var *tab, unsigned int m_p)
     {return new Orbit(m_p);}

    static int privat_tab[];

    static Meta::Base Instance;
 };

// -------------------------------------------------------

class Translate : public Component
 { private:
    int*  mp_type;

    glm::mat4* mp_view;
    glm::mat4  m_view;

    glm::vec3 m_pos;

    glm::vec3 m_target;
    glm::vec3 *m_eye;

    glm::vec4 *mp_dimensions;

    Frustrum  m_frustrum;

    void buildFrustrum(glm::mat4 &matrix);

   public:
    Translate(unsigned int pt);
    ~Translate();

    void linkVar(int def, void* data);

    void doUpdate();

    glm::mat4& getView() { return m_view; }
    Frustrum& getFrustrum() { return m_frustrum; }

    static Object* Create(Lua::Var *tab, unsigned int m_p)
     {return new Translate(m_p);}

    static int privat_tab[];
    static int public_tab[];

    static Meta::Base Instance;
 };

// -------------------------------------------------

class Control : public Component
 { private:
    Lua::State *lvm;

    int* mp_type;

    float *mp_dist;

    glm::vec3 *mp_turn;
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

    void  setDistance(float volume);
    float getDistance();

    void rotation(int x, int y);
    void move(int dir);

    void setType(int tp);
    int  getType();
    
    void setFlag(int flag)       { m_flag = flag; }
    void setCursor(int x, int y) { m_cursor = glm::ivec2(x, y); }

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
