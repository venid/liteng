
#pragma once

#include <string>

class Object;
class Data;
class ResManager;
namespace Lua { class State; }

class Resource
 { public:
    Resource() {}
    virtual ~Resource() { }

   virtual Data* getFile(const std::string& theName) {return nullptr;}
   virtual bool saveFile(const std::string& theName, Data *data) {return false;}
 };

/// ------------------------------------------------------------------------------

class ResDecoder
 {
  protected:
   ResManager *resMgr;
   std::string extension;
  public:
   ResDecoder(ResManager *theManager, const char* ext)
    { resMgr = theManager;
      extension = ".";
      extension += ext;
    }

   virtual ~ResDecoder() {}

   virtual Object* decode(const std::string& theName) = 0;

   bool checkExtension(const std::string& theName)
    { std::string tmp = theName.substr(theName.find_last_of("."));
      if(tmp == extension) return true;
      else return false;
    }
 };
