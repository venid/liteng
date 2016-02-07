
#pragma once

#include "module.h"

namespace Lua { class State; }

class Physics : public Module
 { private:
    double* mp_delta;

    bool init(Lua::State &lua);
    bool msg_processing();
    void set_var();
    void connect();

    int init_update(double);
    int run_update(double);
    int clear_update(double);

   public:
    Physics(const char* Name);
    ~Physics() {}

    static Object* Create(const char* Name, Lua::State* lua)
     { Physics *phys = new Physics(Name);
       phys->init(*lua);
       return phys;
     }

    static Meta::Base Instance;
 };
