#pragma once

#include <vector>
#include "component.h"
#include "vertex.h"
#include "MemoryPool.h"
#include "fbuffer.h"

namespace Lua { class Var; }
namespace Scene { class Render; }
namespace Camera { class Render; class Translate; }
namespace Scene { class World; }

class Shader;
class Mesh;
class Material;
class Node;

namespace Render
{

struct Generic
 { unsigned int id;
   Mesh *mesh;
   Material *mat;
   glm::mat4 transform;
   Generic *next;
 };

// ------------------------------------------------------

class Lists : public Component
 { private:
    Scene::World **mp_world;

    Camera::Translate** mpp_camera;

    Generic **mpp_render;
    Generic *mp_render;
    MemoryPool<Generic> m_pool;

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

// ----------------------------------------------------

class Update : public Component
 { private:
    Generic **mp_render;
    Generic **mp_show;

    Camera::Render** mpp_camera;

   public:
    Update(unsigned int pt);
    ~Update() {}

    void linkVar(int def, void* data);

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Update(m_p);}

    static int public_tab[];
    static int privat_tab[];
    static Meta::Base Instance;
 };

// -------------------------------------------------------

//class ListDraw : public Component
 //{ private:
    //std::vector<Scene::Render*> *mp_scenes;

    //Camera::Render** mpp_camera;

    //Generic **mp_show;
    //MemoryPool<Generic> pool;

   //public:
    //ListDraw(unsigned int pt);
    //~ListDraw() {}

    //void linkVar(int def, void* data);

    //void doUpdate();

    //static Object* Create(Lua::Var* tab, unsigned int m_p)
     //{return new ListDraw(m_p);}

    //static int public_tab[];
    //static int privat_tab[];
    //static Meta::Base Instance;
 //};

// ----------------------------------------------------

class Geometry : public Component
 { private:
    Shader *m_shader;

    FrameBuffer m_fbo;

    Camera::Render** mpp_camera;

    Generic **mp_show;

   public:
    Geometry(Lua::Var& tab, unsigned int pt);
    ~Geometry();

    void linkVar(int def, void* data);

    bool init();
    void clear();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Geometry(*tab, m_p);}

    static int public_tab[];
    static int privat_tab[];
    static Meta::Base Instance;
 };

// -----------------------------------------------------

class Lighting : public Component
 { private:
    Shader *m_shader;

   public:
    Lighting(Lua::Var& tab, unsigned int pt);
    ~Lighting() {}

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Lighting(*tab, m_p);}

    static Meta::Base Instance;
 };

// -------------------------------------------------------

class Compound : public Component
 { private:
    Camera::Render** mpp_camera;

    Generic **mp_show;

   public:
    Compound(unsigned int pt);
    ~Compound();

    void linkVar(int def, void* data);

    bool init();
    void clear();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Compound(m_p);}

    static int public_tab[];
    static int privat_tab[];
    static Meta::Base Instance;
 };

}

