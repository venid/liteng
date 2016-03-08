
#pragma once

#include "component.h"
#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "vertex.h"
#include "MemoryPool.h"
#include "typevar.h"
#include "transform.h"
#include "node.h"

namespace Lua { class Var; }

class Segment;
class ResManager;
class Frustrum;
class Module;

namespace Scene
{

class World : public Component
 { private:
    Node root;
   public:
    World(unsigned int pt);
    ~World() {}

    void linkVar(int def, void* data);

    void connect(Module*);

    void addNode(Object*, int);

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new World(m_p);}

    Node* getNode() { return &root; }

    static int public_tab[];
    static Meta::Base Instance;
 };

// --------------------------------------------------------

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

   // int isVisible(Generic** head, MemoryPool<Generic> &pool,
   //                   Frustrum &frustrum, glm::mat4 &trans);

    static int privat_tab[];
    static int public_tab[];
    static Meta::Base Instance;
 };

// -------------------------------------------------------

class Behavior : public Component
 { private:
    std::vector<Segment*> *mp_segment;
    std::vector<Object*> *mp_scenes;

    Operation* mp_opt;
    Operation m_opt;

    glm::mat4 *mp_trans;
    glm::mat4 m_trans;

    double max_speed;
    double speed;
    float acceleration;
    float start;
    double delta;
    double balance;

    glm::quat m_rot;
    glm::quat m_rotate;
    float acceleration_rot;

   public:
    Behavior(unsigned int pt);
    ~Behavior() {}

    void linkVar(int def, void* data);

    bool init();
    void clear();

    void doUpdate();

    void frame();
    void moveUp();
    void rotate();

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

    Operation* mp_opt;
    std::queue<Operation> m_opt;

   public:
    Control(unsigned int pt);
    ~Control() { }

    void linkVar(int def, void* data);

    bool init();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     { return new Control(m_p); }

    void move(int flag, int dist);
    void rotate(int flag, int param);

    static int privat_tab[];
    static int public_tab[];

    static Meta::Base Instance;
 };

// ------------------------------------------------------

class Build : public Component
 { private:
    std::vector<Segment*> *mp_segment;
    ResManager *mp_res;

    glm::vec3 m_pos;
    glm::mat4 *mp_trans;

   public:
    Build(Lua::Var& tab, unsigned int pt);
    ~Build() {}

    void linkVar(int def, void* data);

    bool init();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Build(*tab, m_p);}

    static int privat_tab[];
    static int public_tab[];
    static Meta::Base Instance;
 };

}
