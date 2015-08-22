
#pragma once

#include <string>
#include <vector>
#include "resource.h"
#include "luastate.h"

#define RES_LOAD 0
#define RES_SAVE 1

class Object;
class Data;

class ResManager
 {
  private:
   std::vector<Resource*>   sources;   // директории ресурсов
   std::vector<ResDecoder*> decoders;  // декодеры файлов
   Resource*                saveRes;   // директория сохранения
   std::vector<Object*>     storage;   //хранилище объектов

  public:
   Lua::State               lvm;

   ResManager(luavm vm);
   ~ResManager();

   int registerSource(Resource *system, bool resType = RES_LOAD);
   int registerDecoder(ResDecoder * loader);

   Data* getResource(const std::string& theName);
   Object* getObject(const std::string& theName);

   void regObject(Object* obj);

   bool saveResource(const std::string& theName, Data *data);

   int cleaning();
 };

typedef ResManager* PResManager;
