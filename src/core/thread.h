/*
 23.04.2014
*/

#ifndef _THREAD_H_
#define _THREAD_H_

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>

class Module;
class Timer;

class Thread
 {
  protected:
   unsigned int m_id;
   std::thread m_thread;
   Timer *m_timer;
   float m_lag;
   std::vector<Module*> m_module;

   std::mutex m_mtx;
   std::condition_variable m_cv;
   std::atomic_int m_count;
   std::atomic_bool m_status;

   void run();
   void clear();
  public:
   Thread();
   virtual ~Thread();

   unsigned int getId() { return m_id; }

   int active();
   int deactive();

   bool isStatus()
    { return m_status.load(std::memory_order_relaxed); }

   virtual void start();
   void addModule(Module *mdl)
    { m_module.push_back(mdl); }

   static void ThreadProc(Thread *thread)
    { thread->run();
      thread->clear();
    }
 };

#endif // _THREAD_H_
