
#include "log.h"
#include <time.h>
#include "timer.h"

Log* Log::m_pSingleton = nullptr;

Log :: Log()
 { m_timer = new Timer;
   m_tm = m_timer->getCurrentTime();
 }

Log :: ~Log()
 { if(m_timer) delete m_timer; }

bool Log :: Init(unsigned char logLevel, const char *pszLogName, const char* title, const char* ver)
 { time_t now;
   struct tm *when;

   if (m_pSingleton == nullptr)
    m_pSingleton = new Log();
   if(m_pSingleton == nullptr) return false;
   m_pSingleton->m_fileName = pszLogName;
   m_pSingleton->m_logLevel = logLevel;
   m_pSingleton->LogSwitch();

   time(&now);
   when = localtime(&now);

   if(when != nullptr)
    { char buff[40];
      strftime(buff, 40, "Started %F  %T", when);
      m_pSingleton->m_ofLog << std::endl << title << std::endl;
      m_pSingleton->m_ofLog << "  Version " << ver << std::endl;
      m_pSingleton->m_ofLog << buff << std::endl;
      m_pSingleton->m_ofLog.flush();
      std::cout << std::endl << title << std::endl;
      std::cout << "  Version " << ver << std::endl;
      std::cout << buff << std::endl;
    }
   return true;
 }

void Log :: Clear()
 { time_t now;
   struct tm *when;

   time(&now);
   when = localtime(&now);

   m_pSingleton->lock();
   if(when != nullptr)
    { char buff[40];
      strftime(buff, 40, "Stopping %F  %T\n", when);
      m_pSingleton->m_ofLog << buff << std::endl;
      m_pSingleton->m_ofLog.flush();

      std::cout << buff << std::endl;
    }
   m_pSingleton->unlock();
   delete m_pSingleton;
 }

void Log :: rec(Level nSev, const char *pszMessage, int line, const char *file)
 { char buffer[128];
   double tm = m_timer->getCurrentTime() - m_tm;

   switch(nSev)
    { case Critical:
       sprintf(buffer, "CRITICAL  [%07.2f]  %s:%d: ", tm, file, line);
       std::cout << "\033[37;1;41m" << buffer << pszMessage << "\033[0m" << std::endl;
       if(IsLogged(nSev)) { m_ofLog << buffer << pszMessage << std::endl; }
       break;
      case Error:
       sprintf(buffer, "ERROR     [%07.2f]  %s:%d: ", tm, file, line);
       std::cout << "\033[37;1;41m" << buffer << pszMessage << "\033[0m" << std::endl;
       if(IsLogged(nSev)) { m_ofLog << buffer << pszMessage << std::endl; }
       break;
      case Warning:
       sprintf(buffer, "WARNING   [%07.2f]  ", tm);
       std::cout << "\033[31;1m" << buffer << pszMessage << "\033[0m" << std::endl;
       if(IsLogged(nSev)) { m_ofLog << buffer << pszMessage << std::endl; }
       break;
      case Debug:
       sprintf(buffer, "DEBUG     [%07.2f]  %s:%d: ", tm, file, line);
       std::cout << "\033[34m" << buffer << pszMessage << "\033[0m" << std::endl;
       if(IsLogged(nSev)) { m_ofLog << buffer << pszMessage << std::endl; }
       break;
      case Info:
       sprintf(buffer, "INFO      [%07.2f]  ", tm);
       std::cout << buffer << pszMessage << std::endl;
       if(IsLogged(nSev)) { m_ofLog << buffer << pszMessage << std::endl; }
       break;
      case Spam:
       sprintf(buffer, "SPAM      [%07.2f]  ", tm);
       std::cout << "\033[32m" << buffer << pszMessage << "\033[0m" << std::endl;
       if(IsLogged(nSev)) { m_ofLog << buffer << pszMessage << std::endl; }
       break;
    }
 }
