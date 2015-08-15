
#include "version.h"
#include "log.h"

#include "data.h"

Version VERSION;

//#pragma pack (push, 1)
struct Var
 { char flag;
   int mas;
   double len;
 };
//#pragma pack (pop)

int main(int argc, char *argv[])
 { if(!Log::Init(Log::Critical | Log::Error | Log::Warning | Log::Debug | Log::Info,
                    "liteng", "----- Little engine -----", VERSION.get())) return 1;

   Data dt;
   short iv = 0xabcd;
   const char* cv = "Hello";
   std::string str("foo");

   Var vr;
   vr.flag = 0x02;
   vr.mas = 1;
   vr.len = 0;
   LOG_INFO("Size struct Var = %i", sizeof(Var));

   dt.add(iv);
   dt.add(cv);
   dt.add(str);
   dt.add(vr);
   dt.setDataFromFile("test.dat");

   Data *pdt = Data::getDataFromFile("test.dat");
   pdt->get(iv);
   LOG_SPAM("iv = %i", iv);
   pdt->get(str);
   LOG_SPAM("cv-str %s", str.c_str());
   pdt->get(str);
   LOG_SPAM("str %s", str.c_str());
   pdt->get(vr);
   LOG_SPAM("vr %i, %i, %f", vr.flag, vr.mas, vr.len);

   Log::Clear();
   return 0;
 }
