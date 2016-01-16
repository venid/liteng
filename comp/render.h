#pragma once

#include <vector>
#include "component.h"
#include "vertex.h"
#include "MemoryPool.h"

namespace Lua { class Var; }
namespace Scene { class Render; }
namespace Camera { class Render; }

class Shader;

namespace Render
{

// ---------------------------------------------------

class Clear : public Component
 { private:

   public:
    Clear(unsigned int pt);
    ~Clear() {}

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Clear(m_p);}

    static Meta::Base Instance;
 };

// ----------------------------------------------------

class ListDraw : public Component
 { private:
    std::vector<Scene::Render*> *mp_scenes;

    Camera::Render** mpp_camera;

    Generic **mp_show;
    MemoryPool<Generic> pool;

   public:
    ListDraw(unsigned int pt);
    ~ListDraw() {}

    void linkVar(int def, void* data);

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new ListDraw(m_p);}

    static int public_tab[];
    static int privat_tab[];
    static Meta::Base Instance;
 };

// ----------------------------------------------------

class MaterialDraw : public Component
 { private:
    Camera::Render** mpp_camera;

    Generic **mp_show;

   public:
    MaterialDraw(unsigned int pt);
    ~MaterialDraw();

    void linkVar(int def, void* data);

    bool init();
    void clear();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new MaterialDraw(m_p);}

    static int public_tab[];
    static int privat_tab[];
    static Meta::Base Instance;
 };

}

