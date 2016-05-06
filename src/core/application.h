#pragma once

#include "thread.h"

class Application : public Thread
 {
   private:
    Application() {}
    ~Application() {}
   public:
    void start();
    static void run();
    static bool init(const char *configFile);
 };
