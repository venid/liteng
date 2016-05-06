
#pragma once

#include <string>
#include "component.h"
#include "input.h"
#include "luastate.h"
#include <lua5.2/lua.hpp>

class Data;

namespace Event
{

class Key : public Component
 { private:
    Lua::State *lvm;
    int m_triger;
    int m_key;
    Data* m_fun;

   public:
    Key(Lua::Var& tab, unsigned int pt);
    ~Key();

    void linkVar(int def, void* data);
    bool init();

    void doUpdate();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Key(*tab, m_p);}

    static int public_tab[];
    static Meta::Base Instance;
 };

// --------------------------------------------------

class Mouse : public Component
 { private:
    Lua::State *lvm;
    int m_triger;
    int m_event;
    Data* m_fun;

   public:
    Mouse(Lua::Var& tab, unsigned int pt);
    ~Mouse();

    void linkVar(int def, void* data);
    bool init();

    void doUpdateMove();
    void doUpdateButton();
    void doUpdateWheele();

    static Object* Create(Lua::Var* tab, unsigned int m_p)
     {return new Mouse(*tab, m_p);}

    static int public_tab[];
    static Meta::Base Instance;
 };

}
