#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "object.h"

class Component;
class Unit;
class Module;

typedef int(Module::* MUpdate)(double);
typedef void(Module::* Observer)(Object*, int);

class Module : public Object
 {
   protected:
    bool action;
    unsigned int thrID;
    unsigned int numQueue;

    std::multimap<unsigned int, Component*> components;
    std::vector<Component*> staticComponents;

    std::multimap<unsigned int,
                  std::pair<Object*, Meta::Method> > msgHandlers;

    struct Link
     { int refCount;
       void* data;
     };
    std::map<int, Link> pool;

    int (Module::*do_update)(double);

    static std::vector<Module*> module_tab;

   public:
    Module(const char* Name);
    ~Module();

    void setThreadID(unsigned int ID) {thrID = ID;}
    unsigned int  getThreadId() {return thrID;}

    void setNumQueue(unsigned int num) {numQueue = num;}
    unsigned int getNumQueue() {return numQueue;}

    virtual void connectMsg() = 0;
    void addMsg(unsigned int msg, Object* obj, const char* theName);

    void addComp(Unit*);
    void delComp();
    void delComp(unsigned int uID);

    void del_var();

    int update(double tm);
    int empty_update(double) { return 0; }

    static unsigned int getModuleID(const char* Name);

    static Meta::Base Instance;
 };

