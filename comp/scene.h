
#pragma once

#include "component.h"
#include <vector>
#include <glm/glm.hpp>
#include "vertex.h"
#include "MemoryPool.h"

namespace Lua { class Var; }

class Segment;
class ResManager;
class Frustrum;

namespace Scene
{

class Render : public Component
 { private:
    std::vector<Segment*> *mp_segment;
    std::vector<Object*> *mp_scenes;

    glm::mat4 *mp_trans;
    glm::mat4 m_trans;

   public:
    Render(unsigned int pt);
    ~Render() {}

    void linkVar(int def, void* data);

    void doUpdate();

    bool init();
    void clear();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Render(m_p);}

    int isVisible(Generic** head, MemoryPool<Generic> &pool,
                      Frustrum &frustrum, glm::mat4 &trans);

    static int privat_tab[];
    static int public_tab[];
    static Meta::Base Instance;
 };

// -------------------------------------------------------

class Behavior : public Component
 { private:
    std::vector<Segment*> *mp_segment;
    std::vector<Object*> *mp_scenes;

    glm::mat4 *mp_trans;
    glm::mat4 m_trans;

   public:
    Behavior(unsigned int pt);
    ~Behavior() {}

    void linkVar(int def, void* data);

    bool init();
    void clear();

    void doUpdate();

    void move();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Behavior(m_p);}

    static int privat_tab[];
    static int public_tab[];
    static Meta::Base Instance;
 };

// -------------------------------------------------------

class Control : public Component
 { private:
    Lua::State *lvm;

   public:
    Control(unsigned int pt);
    ~Control() { }

    void linkVar(int def, void* data);

    bool init();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     { return new Control(m_p); }

//    void move(int dir);

    static int privat_tab[];
    static int public_tab[];

    static Meta::Base Instance;
 };

// ------------------------------------------------------

class Build : public Component
 { private:
    std::vector<Segment*> *mp_segment;
    ResManager *mp_res;

    glm::mat4 *mp_trans;

   public:
    Build(unsigned int pt);
    ~Build() {}

    void linkVar(int def, void* data);

    bool init();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Build(m_p);}

    static int privat_tab[];
    static int public_tab[];
    static Meta::Base Instance;
 };

}
