#include "filesystem.h"
#include "log.h"
#include "data.h"

FileSystem :: FileSystem(const std::string& thePath) : Resource(), path(thePath)
 { LOG_INFO("FileSystem: Created resource source \"%s\"", thePath.c_str()); }

Data* FileSystem :: getFile(const std::string& theName)
 { std::string fileName = path + theName;
   Data *dt = Data :: DataFromFile(fileName.c_str());
   return dt;
 }

bool FileSystem :: saveFile(const std::string& theName, Data *data)
 { std::string fileName = path + theName;
   return data -> DataInFile(fileName.c_str());
 }
