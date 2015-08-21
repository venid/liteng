
#pragma once

#include <string>
#include "resource.h"

class Data;

class FileSystem : public Resource
 {
   private:
    std::string path;

   public:
    FileSystem(const std::string& thePath);

    Data* getFile(const std::string& theName);
    bool saveFile(const std::string& theName, Data *data);
 };
