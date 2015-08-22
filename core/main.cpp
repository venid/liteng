
#include "version.h"
#include "log.h"
#include "application.h"
#include "manager.h"

Version VERSION;

int main(int argc, char *argv[])
 { if(!Log::Init(Log::Critical | Log::Error | Log::Warning | Log::Debug | Log::Info,
                    "liteng", "----- Little engine -----", VERSION.get())) return 1;
   if(!Manager::Init()) return 2;

   const char* conf = argc < 2 ? "config.lua" : argv[1];
   if(Application::init(conf))
    { Manager::Start();
      Application::run();
    }

   Manager::Clear();

   Log::Clear();
   return 0;
 }
