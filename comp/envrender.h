
#pragma once

#include "component.h"

namespace Lua { class Var; }
namespace Camera { class Render; }

class Shader;

namespace Render
{

class EnvDraw : public Component
 { private:
    Shader *m_shader;
    Camera::Render** mpp_camera;

   public:
    EnvDraw(Lua::Var& tab, unsigned int pt);
    ~EnvDraw();

    void linkVar(int def, void* data);

    bool init();
    void clear();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     { return new EnvDraw(*tab, m_p); }

    static int public_tab[];
    static int privat_tab[];

    static Meta::Base Instance;
 };

}
