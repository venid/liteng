
#include "luastate.h"
#include "data.h"

Lua::Var :: Var(Lua::State* lvm, int p, int length)
 { st = lvm;
   ind = p;
   len = length;
   next = nullptr;

   Var* tmp = &st->head;
   while(tmp)
    { if(tmp->next == nullptr)
       { tmp->next = this;
         break;
       }
      tmp = tmp->next;
    }
 }

Lua::Var :: Var(const Lua::Var &var)
 { st = var.st;
   next = nullptr;
   ind = lua_gettop(st->vm) + 1;
   len = var.len;
   for(int i = 0; i < len; i++)
    lua_pushvalue(st->vm, var.ind + i);

   Var *tmp = &st->head;
   while(tmp)
    { if(tmp->next == nullptr)
       { tmp->next = this;
         break;
       }
      tmp = tmp->next;
    }
 }

void Lua::Var :: clear()
 { if(st == nullptr) return;
   Var *tmp = st->head.next;
   if((ind != 0)&&(len > 0))
    { for(int i = 0; i < len; i++)
       lua_remove(st->vm, ind);
      while(tmp)
       { if(tmp->ind > ind) tmp->ind -= len;
         tmp = tmp->next;
       }
    }
   tmp = &st->head;
   while(tmp)
    { if(tmp->next == this)
       { tmp->next = next;
         break;
       }
      tmp = tmp->next;
    }
 }

Lua::Var Lua::Var :: operator[](const char *name)
 { lua_getfield(st->vm, ind, name);
   int n = lua_gettop(st->vm);
   return Var(st, n);
 }

Lua::Var Lua::Var :: operator[](const int index)
 { lua_pushnumber(st->vm, index);
   lua_rawget(st->vm, ind);
   int n = lua_gettop(st->vm);
   return Var(st, n);
 }

Lua::Var& Lua::Var :: operator = (const Lua::Var &var)
 { clear();

   st = var.st;
   next = nullptr;
   ind = lua_gettop(st->vm) + 1;
   len = var.len;
   for(int i = 0; i < len; i++)
    lua_pushvalue(st->vm, var.ind + i);

   Var *tmp = &st->head;
   while(tmp)
    { if(tmp->next == nullptr)
       { tmp->next = this;
         break;
       }
      tmp = tmp->next;
    }

   return *this;
 }

Lua::Var :: operator int() const
 { return (int)lua_tointegerx(st->vm, ind, nullptr); }

Lua::Var :: operator unsigned int() const
 { return (unsigned int)lua_tounsignedx(st->vm, ind, nullptr); }

Lua::Var :: operator unsigned long long int() const
 { return (unsigned long long int)lua_tonumber(st->vm, ind);}

Lua::Var :: operator float() const
 { return (float)lua_tonumber(st->vm, ind); }

Lua::Var :: operator double() const
 { return (double)lua_tonumber(st->vm, ind); }

Lua::Var :: operator std::string() const
 { return (std::string)lua_tostring(st->vm, ind); }

Lua::Var :: operator Object*() const
 { return (Object*)lua_touserdata(st->vm, ind); }

Lua::Var :: operator Object**() const
 { return (Object**)lua_touserdata(st->vm, ind); }

Lua::Var :: operator Data*() const
 { return (Data*)lua_touserdata(st->vm, ind); }


bool Lua::Var :: operator == (Lua::Type tp)
 { if(ind != 0) return lua_type(st->vm, ind) == tp;
   else return false;
 }

bool Lua::Var :: operator != (Lua::Type tp)
 { if(ind != 0) return lua_type(st->vm, ind) != tp;
   else return false;
 }

Lua::Var& Lua::Var :: operator ++ ()
 { if(len < 2) return *this;
   lua_remove(st->vm, ind);
   len--;

   Var *tmp = st->head.next;
   while(tmp)
    { if(tmp->ind > ind) tmp->ind--;
      tmp = tmp->next;
    }
   return *this;
 }

int Lua::Var :: type(int offset)
 { if(st) return lua_type(st->vm, ind + offset);
   else return Lua::NIL;
 }

int Lua::Var :: size(int offset)
 { if(lua_type(st->vm, ind + offset) == LUA_TTABLE)
    return lua_rawlen(st->vm, ind + offset);
   else return 0;
 }

void Lua::Var :: reg_object(const char* name, Object* obj)
 { if(lua_type(st->vm, ind) == LUA_TTABLE)
    { void** udata = (void**)lua_newuserdata(st->vm, sizeof(void*));
      *udata = obj;
      lua_setfield(st->vm, ind, name);
    }
 }

void Lua::Var :: reg_object(const int index, Object* obj)
 { if(lua_type(st->vm, ind) == LUA_TTABLE)
    { _push(st->vm, index);
      void** udata = (void**)lua_newuserdata(st->vm, sizeof(void*));
      *udata = obj;
      lua_settable(st->vm, ind);
    }
 }

//Lua::Var Lua::Var :: cut(int offset)
 //{ if(len < 2) return Var();
   //else
    //{ 

    //}
 //} 

// ----------------------------------------------------------------------------

Lua::Var Lua::State :: load_file(const char* fileName)
 { int n = lua_gettop(vm);
   if(luaL_dofile(vm, fileName))
    lua_error(vm);
   int t = lua_gettop(vm) - n;
   if (t > 0) return Var(this, n + 1, t);
   else return Var();
 }

Lua::Var Lua::State :: load_data(Data* data)
 { int n = lua_gettop(vm);
   if(luaL_loadbuffer(vm, (const char*)data -> getPtr(), data ->getLength(),
                           "") || lua_pcall(vm, 0, LUA_MULTRET, 0))
    lua_error(vm);
   int t = lua_gettop(vm) - n;
   if (t > 0) return Var(this, n + 1, t);
   else return Var();
 }

Lua::Var Lua::State :: load_str(std::string &str)
 { int n = lua_gettop(vm);
   if(luaL_dostring(vm, str.c_str()))
    lua_error(vm);
   int t = lua_gettop(vm) - n;
   if (t > 0) return Var(this, n + 1, t);
   else return Var();
 }

// --------------------------------------------------------------------------------

Lua::Thread :: Thread(Lua::State* st)
 { state = st;
   thr = lua_newthread(state->vm);
   id = luaL_ref(state->vm, LUA_REGISTRYINDEX);
 }

Lua::Thread :: ~Thread()
 { luaL_unref(state->vm, LUA_REGISTRYINDEX, id); }

