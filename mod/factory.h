#pragma once

#include <string>
#include <vector>
#include "module.h"
#include "luastate.h"

class ResManager;
class Unit;

class Factory : public Module
 { private:
    std::vector<Unit*> entity;
    std::vector<std::string> s_data;
    Lua::State lvm;
    static ResManager *resManager;

    bool init(Lua::State &lua);
    void set_var();

    int init_update(double);
    int run_update(double);
    int clear_update(double);

    static int res_load_object(luavm vm);
    static int res_load_data(luavm vm);
    static int res_load_script(luavm vm);
    static int send_message(luavm vm);
    static int create_unit(luavm vm);

   public:
    Factory(const char* Name);
    ~Factory();

    void connectMsg();
    void clear(Object*, int);
    void add(Object*, int);
    void make(Object*, int);
    void test(Object*, int);

    static Object* Create(const char* Name, Lua::State* lua)
     { Factory *fact = new Factory(Name);
       fact->init(*lua);
       return fact;
     }

    static Meta::Base Instance;
 };

