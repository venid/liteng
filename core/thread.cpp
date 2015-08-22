
#include "thread.h"
#include "log.h"
#include "timer.h"
#include "object.h"
#include "module.h"
#include <algorithm>
#include <chrono>


Thread::Thread() : m_thread(), m_count(0), m_status(false)
 { m_id = Object::genID();
   m_timer = new Timer();
   m_lag = 0.02f;
   LOG_INFO("Thread: Create id - %i", m_id);
 }

Thread::~Thread()
 { if(m_timer) delete m_timer;
   LOG_INFO("Thread: Delete id - %i", m_id);
 }

void Thread :: start()
 { m_timer->getPeriodTime();
   m_status.store(true, std::memory_order_relaxed);
   m_thread = std::thread(ThreadProc, this);
   m_thread.detach();
 }

int Thread :: active()
 { if(m_count.load(std::memory_order_relaxed) == 0)
    { m_count++;
      m_cv.notify_one();
    }
   else m_count++;
   return m_count.load(std::memory_order_relaxed);
 }

int Thread :: deactive()
 { m_count--;
   return m_count.load(std::memory_order_relaxed);
 }

void Thread :: run()
 { double curTime = m_lag;
   int act;
   std::unique_lock<std::mutex> lck(m_mtx);
   while(true)
    { curTime += m_timer->getPeriodTime();

      if (curTime >= m_lag)
       { act = 0;
         //LOG_SPAM("Thread: Run id - %i, period = %f", m_id, curTime);
         std::for_each(m_module.begin(), m_module.end(),[&act, curTime](Module* smd)
          { act += smd -> update(curTime); });
         curTime = 0.0;
         if(act == 0) return;
         if(m_count.load(std::memory_order_relaxed) == 0)
          m_cv.wait(lck);
       }
      else if(curTime < (m_lag * 0.9f))
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(m_lag - curTime)*1000));
    }
 }

void Thread :: clear()
 { std::for_each(m_module.begin(), m_module.end(), [](Module* smd){smd->release();});
   m_module.clear();
   LOG_INFO("Thread: Clear id - %i", m_id);
   m_status.store(false, std::memory_order_relaxed);
 }

