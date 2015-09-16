
#pragma once

#include <vector>
#include "component.h"

namespace Lua { class Var; }
namespace Scene { class Behavior; }

namespace Dynamics
{

class Scenes : public Component
 { private:
    double* mp_delta;
    double m_rest;

    std::vector<Scene::Behavior*> *mp_scenes;

   public:
    Scenes(unsigned int pt);
    ~Scenes() {}

    void linkVar(int def, void* data);

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Scenes(m_p);}

    static int public_tab[];
    static int privat_tab[];
    static Meta::Base Instance;
 };

}
