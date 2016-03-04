
#pragma once

#include "module.h"
#include "luastate.h"
#include <X11/Xlib.h>

class Control : public Module
 { private:
    Lua::State lvm;
    Display *m_dpy;

    bool init(Lua::State &lua);
    void set_var();

    int init_update(double);
    int run_update(double);

    static int send_message(luavm vm);

   public:
    Control(const char* Name);
    ~Control();

    void connectMsg();
    void clear(Object*, int);
    void add(Object*, int);
    void addScript(Object*, int);

    static Object* Create(const char* Name, Lua::State* lua)
     { Control *control = new Control(Name);
       control->init(*lua);
       return control;
     }

    static Meta::Base Instance;
 };
