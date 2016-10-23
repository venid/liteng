
#include "log.h"
#include <time.h>

Log :: Log()
 { timespec curTime;
   clock_gettime(CLOCK_MONOTONIC, &curTime);
   m_tm = (double)curTime.tv_sec + (double)curTime.tv_nsec * 0.000000001;

 }

bool Log :: Init(unsigned char logLevel, const char *pszLogName, const char* title)
 { time_t now;
   struct tm *when;

   GetRef().m_fileName = pszLogName;
   GetRef().m_logLevel = logLevel;
   GetRef().LogSwitch();

   time(&now);
   when = localtime(&now);

   if(when != nullptr)
    { char buff[40];
      strftime(buff, 40, "Started %F  %T", when);
      GetRef().m_ofLog << std::endl << title << std::endl;
      GetRef().m_ofLog << buff << std::endl;
      GetRef().m_ofLog.flush();
      std::cout << std::endl << title << std::endl;
      std::cout << buff << std::endl;
    }
   return true;
 }

void Log :: Clear()
 { time_t now;
   struct tm *when;

   time(&now);
   when = localtime(&now);

   GetRef().lock();
   if(when != nullptr)
    { char buff[40];
      strftime(buff, 40, "Stopping %F  %T\n", when);
      GetRef().m_ofLog << buff << std::endl;
      GetRef().m_ofLog.flush();

      std::cout << buff << std::endl;
    }
   GetRef().m_logLevel = 0;
   GetRef().unlock();
 }

void Log :: rec(Level nSev, const char *pszMessage, int line, const char *file)
 { char buffer[128];
   timespec curTime;
   clock_gettime(CLOCK_MONOTONIC, &curTime);
   double tm = (double)curTime.tv_sec + (double)curTime.tv_nsec * 0.000000001 - m_tm;

   switch(nSev)
    { case Error:
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
