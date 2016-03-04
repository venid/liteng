
#pragma once

#include <stdio.h>

/**
  \brief Класс для отслеживания версии продукта

  Версия продукта состоит из 4 цифр.

  1 MAJOR Определяе основную версию продукта 
          Изменяется в ручную. MINOR и PATCH при этом ставятся в 0.

  2 MINOR Определяет доплнительный функционал продукта в пределах одной основной версии.
          Изменяется при слиянии веток в git. PATCH устанавливается в 0.

  3 PATCH Определяет исправление и опьемизации в пределах MINOR версии продукта.
          Изменяется при коммитах в git.

  4 BUILD Счетчик сборок продукта.
          Значение изменяется автоматически после каждой удачной сборке.
*/

class Version
 { private:
    unsigned char  MAJOR = 0;
    unsigned short MINOR = 2;
    unsigned short PATCH = 0;
    unsigned int   BUILD = 746; // Это значение будет изменено автоматически
    char buf[32];
   public:
    /// \brief Конструктор класса создает строку версии
    Version()
     { sprintf(buf, "%02i.%02i.%02i.%i", MAJOR, MINOR, PATCH, BUILD); }

    /** \brief возвращаем строку с версией продукта

        Строка имеет вид 00.00.00.0
    */
    const char* get() { return buf;}
 };

extern Version VERSION;
