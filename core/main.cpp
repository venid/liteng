
#include "version.h"
#include "log.h"

#include "luastate.h"

Version VERSION;

int main(int argc, char *argv[])
 { if(!Log::Init(Log::Critical | Log::Error | Log::Warning | Log::Debug | Log::Info,
                    "liteng", "----- Little engine -----", VERSION.get())) return 1;

// проверка coroutin в lua

   Lua::State state;
   Lua::Thread thr = state.thread();

   state.load_file("test.lua");
   Lua::Var fun = state["fun"];
   if(fun == Lua::FUN) LOG_SPAM("state.top = %i", state.top());

   Lua::Var tmp = thr.run(fun, 42);
   int i = tmp;
   LOG_INFO("thr.status = %i, thr.result = %i state.top = %i",thr.status(), i, state.top());

   tmp = thr.resume(32);
   i = tmp;
   LOG_INFO("thr.status = %i, thr.result = %i state.top = %i",thr.status(), i, state.top());

   tmp = thr.resume(22);
   i = tmp;
   LOG_INFO("thr.status = %i, thr.result = %i state.top = %i",thr.status(), i, state.top());

   Log::Clear();
   return 0;
 }
