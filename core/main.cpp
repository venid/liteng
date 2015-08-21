
#include "version.h"
#include "log.h"

#include "any.h"
#include <vector>
#include <string>

Version VERSION;

int main(int argc, char *argv[])
 { if(!Log::Init(Log::Critical | Log::Error | Log::Warning | Log::Debug | Log::Info,
                    "liteng", "----- Little engine -----", VERSION.get())) return 1;

   std::vector<Any> vec;

   vec.push_back(42);
   vec.push_back("test");
   vec.push_back(12.3f);

   if(vec[0].is<int>())
    LOG_INFO("vec[0] %i", (int)vec[0]);

   LOG_INFO("vec[1] %s", (const char*)vec[1]);

   if(vec[2].is<float>())
    LOG_INFO("vec[2] %f", (float)vec[2]);

   Log::Clear();
   return 0;
 }
