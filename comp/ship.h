
#pragma once

#include "component.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Lua { class Var; }

class ResManager;
class Segment;
class Mesh;

namespace Ship
{

class Make : public Component
 { public:
    struct Cell
     { char types;
       char face[6];
       char point[8];
     };
   private:
    std::vector<Segment*> *mp_segment;
    ResManager *mp_res;

    glm::mat4 *mp_trans;

    std::string m_blueprint;

    void primary_form(glm::vec3 &offset);
    unsigned int set_flags();
    void build(Mesh* mesh, unsigned int sz);

    void frame1(int x, int y, int z, glm::vec3* points, float scale);
    void rotateX(Cell* cell, int n);
    void rotateY(Cell* cell, int n);
    void rotateZ(Cell* cell, int n);
    void face_ratio(char &f1, char &f2);
    int triangles(char face, glm::ivec4 &pt, glm::ivec3& tr1, glm::ivec3 &tr2);

    void face_box(Cell *cell);

   public:
    Make(Lua::Var &tab, unsigned int pt);
    ~Make() {}

    void linkVar(int def, void* data);

    bool init();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     { return new Make(*tab, m_p); }

    static int privat_tab[];
    static int public_tab[];

    static Meta::Base Instance;
 };

}
