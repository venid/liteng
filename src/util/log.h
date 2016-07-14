/*
  03.04.2014
*/
#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <mutex>
#include <assert.h>

class Timer;

class Log
 {
  public:
   enum Level : unsigned char
    { Error    = 0x01,
      Warning  = 0x02,
      Debug    = 0x04,
      Info     = 0x08,
      Spam     = 0x10
    };

   static bool Init(unsigned char logLevel, const char *pszLogName, const char* title);

   static void Clear();

   static Log& GetRef() { return *m_pSingleton; }

   void rec_format(Level nSev, int line, const char *file, const char *pszFormat, ...)
    { char buffer[512];
      va_list va;

      lock();
      va_start(va, pszFormat);
      vsprintf(buffer, pszFormat, va);
      va_end(va);
      rec(nSev, buffer, line, file);
      unlock();
    }

  protected:
   static Log   *m_pSingleton;
   std::string   m_fileName;
   std::ofstream m_ofLog;
   unsigned char m_logLevel = 0;
   std::mutex    m_mutex;

   double        m_tm;
   Timer*        m_timer;

   Log();
   ~Log();

   void LogSwitch()
    { std::string path = m_fileName + ".log";
      if(!m_ofLog.is_open())
       m_ofLog.open(path.c_str(), std::ios_base::out | std::ios_base::app);
    }

   void lock() { m_mutex.lock(); }
   void unlock() { m_mutex.unlock(); }

   bool IsLogged(Level severity) { return (m_logLevel & severity); }

   void rec(Level nSev, const char *pszMessage, int line, const char *file);
 };

#define LOG_ERROR(str, ...) \
 Log::GetRef().rec_format(Log::Error, __LINE__, __FILE__, str, ##__VA_ARGS__)

#define LOG_WARNING(str, ...) \
 Log::GetRef().rec_format(Log::Warning, __LINE__, __FILE__, str, ##__VA_ARGS__)

#ifdef DEBUG
#define LOG_DEBUG(str, ...) \
 Log::GetRef().rec_format(Log::Debug, __LINE__, __FILE__, str, ##__VA_ARGS__)
#else
#define LOG_DEBUG(str, ...)
#endif

#define LOG_INFO(str, ...) \
 Log::GetRef().rec_format(Log::Info, __LINE__, __FILE__, str, ##__VA_ARGS__)

#define LOG_SPAM(str, ...) \
 Log::GetRef().rec_format(Log::Spam, __LINE__, __FILE__, str, ##__VA_ARGS__)

#endif //LOG_H_
