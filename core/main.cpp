
#include "version.h"
#include "log.h"

Version VERSION;

int main(int argc, char *argv[])
 { if(!Log::Init(Log::Critical | Log::Error | Log::Warning | Log::Debug | Log::Info,
                    "liteng", "----- Little engine -----", VERSION.get())) return 1;

   const char* ts = "test";
   LOG_ERROR("Error %s", ts);
   LOG_WARNING("Warning %s", ts);
   LOG_DEBUG("Debug %s", ts);
   LOG_INFO("Info %s", ts);
   LOG_SPAM("Spam %s", ts);

   Log::Clear();
   return 0;
 }
