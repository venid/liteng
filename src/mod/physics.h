
#pragma once

#include "module.h"

namespace Lua { class State; }

class Physics : public Module
 { private:
    double* mp_delta;

    bool init(Lua::State &lua);
    void set_var();

    int init_update(double);
    int run_update(double);

   public:
    Physics(const char* Name);
    ~Physics() {}

    void connectMsg();
    void clear(Object*, int);
    void add(Object*, int);

    static Object* Create(const char* Name, Lua::State* lua)
     { Physics *phys = new Physics(Name);
       phys->init(*lua);
       return phys;
     }

    static Meta::Base Instance;
 };
