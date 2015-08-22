#pragma once

#include <map>
#include <string>
#include <vector>
#include "object.h"

#include "log.h"

class Component;
class Unit;
class Module;

typedef int(Module::* MUpdate)(double);

class Module : public Object
 {
   protected:
    bool action;
    unsigned int thrID;
    unsigned int numQueue;
    unsigned int* tabMessages;

    std::multimap<unsigned int, Component*> components;

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
    unsigned int* getMsgList()  {return tabMessages;}

    void setNumQueue(unsigned int num) {numQueue = num;}
    unsigned int getNumQueue() {return numQueue;}

    void addComp(Unit*);
    void delComp();
    void delComp(unsigned int uID);

    void del_var();

    int update(double tm) { return (this->*do_update)(tm); }
    int empty_update(double) { return 0; }

    static unsigned int getModuleID(const char* Name);

    static Meta::Base Instance;
    static unsigned int messages_list[];

 };

