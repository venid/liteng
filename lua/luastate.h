
#pragma once

#include <lua5.2/lua.hpp>
#include <string>

typedef lua_State* luavm;

class Data;
class Object;

namespace Lua
{

class State;
class Var;
class Thread;

enum Type
 { NIL  = 0,
   BOOL,
   LDATA,
   NUM,
   STR,
   TAB,
   FUN,
   UDATA,
   THR
 };

const bool META = true;

inline void _push(luavm vm, const int& arg)
 { lua_pushinteger(vm, arg); }
inline void _push(luavm vm, const float& arg)
 { lua_pushnumber(vm, arg); }
inline void _push(luavm vm, const unsigned int& arg)
 { lua_pushunsigned(vm, arg); }
inline void _push(luavm vm, const double& arg)
 { lua_pushnumber(vm, arg); }
inline void _push(luavm vm, const bool& arg)
 { lua_pushboolean(vm, arg ? 1 : 0); }
inline void _push(luavm vm, const std::string& arg)
 { lua_pushstring(vm, arg.c_str()); }
inline void _push(luavm vm, lua_CFunction arg)
 { lua_pushcfunction(vm, arg); }
inline void _push(luavm vm, const Var& arg);

template<typename Tp>
inline void _push(luavm vm, const Tp arg)
 { auto tmp = arg;
   lua_pushlightuserdata(vm, tmp);
 }

inline void _set(luavm vm) { }
template<typename Tp>
inline void _set(luavm vm, const Tp& arg) { _push(vm, arg); }
template <typename Tp, typename... Args>
inline void _set(luavm vm, const Tp& first, const Args&... second)
 { _push(vm, first);
   _set(vm, second...);
 }

// ---------------------------------------------------------

class Var
 { private:
    int ind;
    int len;
    Var *next;
    State *st;

    void clear();

   public:
    Var()
     { st = nullptr;
       ind = 0;
       len = 0;
       next = nullptr;
     }
    Var(State *lvm, int p, int length = 1);
    Var(const Var &var);
    ~Var() { clear(); }

    operator int() const;
    operator unsigned int() const;
    operator unsigned long long int() const;
    operator float() const;
    operator double() const;
    operator std::string() const;
    operator Object*() const;
    operator Object**() const;
    operator Data*() const;

    Var operator [] (const char *name);
    Var operator [] (const int index);

    Var& operator = (const Var &var);

    bool operator == (Type);
    bool operator != (Type);
    Var& operator ++ ();

    template<typename... Tp>
    Var operator () (Tp... args);

    int size(int offset = 0);
    int type(int offset = 0);

    int length() { return len; }
    //Var cut(int offset);

    template<typename Tp>
    void reg(const char* name, Tp& arg);
    template<typename Tp>
    void reg(const int index, Tp& arg);

    void reg_object(const char* name, Object* obj);
    void reg_object(const int index, Object* obj);

    friend class State;
    friend class Thread;
    friend void _push(luavm vm, const Var& arg);
 };

inline void _push(luavm vm, const Var& arg)
 { lua_pushvalue(vm, arg.ind); }

// ----------------------------------------------------------

class Thread
 { private:
    State* state;
    luavm thr;
    int id;

   public:
      Thread(State* st);
     ~Thread();

    inline int status() { return lua_status(thr); }

    template<typename... Tp>
    Var run(Var& fun, Tp... args);

    template<typename... Tp>
    Var resume(Tp... args);

    friend class State;
 };

// --------------------------------------------------------------

class State
 { private:
    bool valid;
    Var head;

   public:
    luavm vm;
    friend class Var;

    State() : head()
     { vm = luaL_newstate();
       if(vm)
        { luaL_openlibs(vm);
          valid = true;
          head.st = this;
        }
     }

    State(luavm lvm) : head()
     { vm = lvm;
       valid = false;
       head.st = this;
     }

    ~State()
     { if(valid && vm) lua_close(vm); }

    Var load_file(const char* fileName);
    Var load_data(Data* data);
    Var load_str(std::string &str);

    Var operator[](const char *name)
     { lua_getglobal(vm, name);
       int n = lua_gettop(vm);
       return Var(this, n);
     }

    Var operator[](const int index)
     { lua_pushvalue(vm, index);
       return Var(this, lua_gettop(vm));
     }

   template<typename Tp>
   void reg(const char* name, Tp& arg)
    { _push(vm, arg);
      lua_setglobal(vm, name);
    }

   void reg_object(const char* name, Object* obj, const char* meta)
    { void** udata = (void**)lua_newuserdata(vm, sizeof(void*));
      *udata = obj;
      luaL_getmetatable(vm, meta);
      lua_setmetatable(vm, -2);
      lua_setglobal(vm, name);
    }

   template<typename... Tp>
   int ret(Tp... args)
    { int sz = sizeof...(args);
       _set(vm, args...);
      return sz;
    }

   Var sig()
    { int n = lua_gettop(vm);
      if(n > 0) return Var(this, 1, n);
      else return Var();
    }

   Var push_nil()
    { lua_pushnil(vm);
      return Var(this, lua_gettop(vm));
    }

   inline int top() { return lua_gettop(vm); }
   inline int status() { return lua_status(vm); }

   template<typename... Tp>
   void yield(Tp... args)
    { int sz = sizeof...(args);
      _set(vm, args...);
      lua_yield(vm, sz);
    }

   inline Thread thread() { return Thread(this); }

   inline Var new_tab()
    { lua_newtable(vm);
      return Var(this, lua_gettop(vm));
    }

   Var new_tab(const char* name, bool meta = false)
    { if(meta == false)
       { lua_newtable(vm);
         lua_setglobal(vm, name);
         lua_getglobal(vm, name);
         return Var(this, lua_gettop(vm));
       }
      else
       { luaL_newmetatable(vm, name);
         return Var(this, lua_gettop(vm));
       }
    }

 };

// --------------------------------------------------------------

template<typename... Tp>
Var Var :: operator () (Tp... args)
 { int sz = sizeof...(args);
   int tp = lua_gettop(st->vm);
   lua_pushvalue(st->vm, ind);
   _set(st->vm, args...);
   if(lua_pcall(st->vm, sz, LUA_MULTRET, 0) == LUA_OK)
    return Var(st, tp + 1, lua_gettop(st->vm) - tp);
   else return Var();
 }

template<typename Tp>
void Var :: reg(const char* name, Tp& arg)
 { if(lua_type(st->vm, ind) == LUA_TTABLE)
    { _push(st->vm, arg);
      lua_setfield(st->vm, ind, name);
    }
 }

template<typename Tp>
void Var :: reg(const int index, Tp& arg)
 { if(lua_type(st->vm, ind) == LUA_TTABLE)
    { _push(st->vm, index);
      _push(st->vm, arg);
      lua_settable(st->vm, ind);
    }
 }

template<typename... Tp>
Var Thread :: run(Var& fun, Tp... args)
 { int sz, top;

   if(lua_status(thr) == LUA_OK)
    { sz = sizeof...(args);
      lua_pushvalue(fun.st->vm, fun.ind);
      lua_xmove(fun.st->vm, thr, 1);
      _set(thr, args...);
      lua_resume(thr, state->vm, sz);
      top = lua_gettop(thr);
      if(top > 0)
       { lua_xmove(thr, state->vm, top);
         return Var(state, lua_gettop(state->vm) - top + 1, top);
       }
    }
   return Var();
 }

template<typename... Tp>
Var Thread :: resume(Tp... args)
 { int sz, top;

   if(lua_status(thr) == LUA_YIELD)
    { sz = sizeof...(args);
      _set(thr, args...);
      lua_resume(thr, state->vm, sz);
      top = lua_gettop(thr);
      if(top > 0)
       { lua_xmove(thr, state->vm, top);
         return Var(state, lua_gettop(state->vm) - top + 1, top);
       }
    }
   return Var();
 }

}

