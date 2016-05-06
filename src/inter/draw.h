#pragma once

#include "component.h"
#include "MemoryPool.h"
#include <glm/glm.hpp>

namespace Lua { class Var; }
namespace Camera { class Render; }
namespace Scene { class World; }

class Shader;
class Node;
class Mesh;

namespace Interface
{

struct Scope
 { unsigned int id;
   glm::vec4 color;
   glm::mat4 transform;
   Scope *next;
 };

class Lists : public Component
 { private:
    Scene::World **mp_world;

    Scope** mpp_border;
    Scope*  mp_border;
    MemoryPool<Scope> m_pool;

    void createList(Node* node, glm::mat4 &trans);

   public:
    Lists(unsigned int pt);
    ~Lists() {}

    void linkVar(int def, void* data);

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Lists(m_p);}

    static int public_tab[];
    static int privat_tab[];
    static Meta::Base Instance;

 };

// -------------------------------------------------------

class Border : public Component
 { private:
    Shader* m_shader;
    Mesh*   m_box;

    Camera::Render** mpp_camera;

    Scope** mpp_border;
    Scope*  mp_border;

   public:
    Border(Lua::Var& tab, unsigned int pt);
    ~Border();

    void linkVar(int def, void* data);

    bool init();
    void clear();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     { return new Border(*tab, m_p); }

    static int public_tab[];
    static int privat_tab[];

    static Meta::Base Instance;
 };

}
