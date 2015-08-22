#pragma once

#include <map>
#include <vector>
#include <queue>
#include <mutex>
#include <unistd.h>
#include "thread.h"
#include "object.h"

class Module;

class Manager
 { private:
    struct Message
     { unsigned int hValue;
       Object* data;
       int Param;
     };

    struct QModule
     { Thread *thr;
       bool active;
       std::queue<Message> qmes;
     };

    static Manager *msgMan;

    std::mutex m_mtx;
    std::vector<Thread*> threads;
    std::vector<QModule*> qModule;
    std::multimap<unsigned int, unsigned int> msgTab;

    Manager() {}
    ~Manager() {}

    void start();
    unsigned int add_thread(Thread* thr);
    void add_module(Module* mdl);
    void reg_message(Module* mdl);
    int create_queue(Thread* thr);
    bool clear(); // delete all items

    bool handlerMsg(int msg, Object* item, int param);
    int put(int msg, Object* item, int param);
    bool get(unsigned key, int &msg, Object* &obj, int &param);

    inline void lock() { m_mtx.lock(); }
    inline void unlock() { m_mtx.unlock(); }

   public:

    static bool Init();
    static void Clear();
    static void Start() { msgMan->start(); };

    static unsigned int addThread(Thread* thr);
    static void addModule(Module* mdl);

    static int sendMessage(int Msg, Object* obj, int param)
     { int n;
       msgMan->lock();
       n = msgMan->put(Msg, obj, param);
       msgMan->unlock();
       return n;
     }

    static bool getMessage(int numQ, int &Msg, Object* &obj, int &param)
     { bool result;
       msgMan->lock();
       result = msgMan->get(numQ, Msg, obj, param);
       msgMan->unlock();
       return result;
     }
 };

