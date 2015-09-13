
#pragma once

#include "component.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Lua { class Var; }

class ResManager;
class Segment;
class Mesh;

namespace Space
{

class Body : public Component
 { private:
    std::vector<Segment*> *mp_segment;
    ResManager *mp_res;

    std::string m_fun;

    void primary_form(glm::vec3 &offset);
    void noise(glm::vec3 &offset);
    unsigned int set_flags();
    void create_point();
    void create_normal();
    void build(Mesh* mesh, unsigned int sz, float scale);

   public:
    Body(Lua::Var &tab, unsigned int pt);
    ~Body() {}

    void linkVar(int def, void* data);

    bool init();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     { return new Body(*tab, m_p); }

    static int privat_tab[];
    static int public_tab[];

    static Meta::Base Instance;
 };

}
