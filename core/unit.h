
#pragma once

#include <map>
#include <vector>
#include <string>
#include "object.h"

class Component;

class Unit : public Object
 {
   private:
    std::map<int, void*> pool;
    std::vector<Component*> comp;
   public:

    Unit(const char *theName);
    ~Unit();

    void addComp(Component*);

    bool remove();

    static Meta::Base Instance;

    friend class Module;
 };
