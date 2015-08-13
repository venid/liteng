/*
 07.04.2014
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <time.h>
#include <unistd.h>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <thread>

/**
  \brief Класс для замера временных промежутков.

  Используется высокоточный таймир с отсчетом от старта операционной системы.
  Требуется линковка с библиотекой -lrt
  Не является потоко безопасным.
*/

class Timer
 {
  private:
   bool isCounterStarted;
   bool isCounterWorkComplete;

   timespec m_start, m_end, m_period;

   double getRealCount(timespec time) ///< Переводит тип timespec в секунды
    { return (double)time.tv_sec + (double)time.tv_nsec * 0.000000001; }

  public:
   Timer()
    { isCounterStarted = false;
      isCounterWorkComplete = false;
      m_period.tv_sec = 0;
      m_period.tv_nsec = 0;
    }
    ~Timer() {}

    /**
      \breif Старт отсчета замеряемого интервала.

      Повторный вызов функции, до вызова stop, будет проигнорирован и вернет false.
    */
    bool start()
     { if(!isCounterStarted)
        { isCounterStarted = true;
          isCounterWorkComplete = false;
          clock_gettime(CLOCK_MONOTONIC, &m_start);
          return true;
        }
       else return false;
     }

    /**
     \breif Остановка замера интервала времени.

     Вызов функции, до вызова start, будет проигнорирован и вернет false.
    */
    bool stop()
     { if(isCounterStarted)
        { clock_gettime(CLOCK_MONOTONIC, &m_end);
          isCounterStarted = false;
          isCounterWorkComplete = true;
          return true;
        }
       else return false;
     }

    /**
      \brief Функция приостанавливает выполнение потока на заданный промежуток времени.

      Время паузы задается в секундах с точностью до милисекунд.
    */
    void pause(double sec)
     { unsigned int tmp = floor(sec * 1000);
       std::this_thread::sleep_for(std::chrono::milliseconds(tmp));
     }

    /**
     \breif Функция возвращает время замеряемого интервала в секундах.

     Замеряемый интервал определяется функциями start и stop.
     Вызов функции во время замера вернет 0
    */
    double getSampleTime()
     { if(isCounterWorkComplete)
        return getRealCount(m_end) - getRealCount(m_start);
       else return 0;
     }

    /// \breif Функция возвращает время прошедшие со старта системы (в секундах).
    double getCurrentTime()
     { timespec currentTime;
       clock_gettime(CLOCK_MONOTONIC, &currentTime);
       return getRealCount(currentTime);
     }

    /// \breif Функция возвращает время прошедшие с прошлого вызова данной функции.
    double getPeriodTime()
     { timespec tmp = m_period;
       clock_gettime(CLOCK_MONOTONIC, &m_period);
       return getRealCount(m_period) - getRealCount(tmp);
     }

    /// \breif Функция возвращает мировое время в секундах.
    static uint64_t getWorldTime()
     { return time(nullptr); }
 };

#endif
