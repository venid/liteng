
#pragma once

#include "object.h"
#include "luastate.h"

class Unit;
class Component;

typedef void(Component::* CUpdate)();

class Component : public Object
 {
   protected:
    unsigned int m_point;
    unsigned int m_id;

    CUpdate m_update;

    Unit* m_unit;

    int m_im;
    static int getting(luavm vm);
    static int calling(luavm vm);

   public:

    Component(unsigned int point);
    ~Component();

    unsigned int getPoint() const { return m_point; }

    void setOwner(Unit *un) {m_unit = un;}
    unsigned int getUnitId();

    void setModuleId(unsigned int ID) { m_id = ID; }
    unsigned int getModuleId() { return m_id; }

    void regComp(Lua::State& st)
     { st.reg_object(getName(), this, "Component"); }

    virtual bool init()  { return true; }
    virtual void clear() {}

    virtual void linkVar(int def, void* data) {}

    void update() {(this->*m_update)();}
    void doUpdate() {}

    int *privat_var;
    int *public_var;

    static void RegMeta(Lua::State& st);

    static int empty_tab[];

    static Meta::Base Instance;
 };
