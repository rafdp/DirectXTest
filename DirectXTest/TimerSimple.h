//!
//! \file    TimerSimple.h
//! \brief   Timer class.
//!
//! \version [1.1 Simple]
//! \author  Copyright (C) Rafdp (Raffaele Della Pietra)
//! \date    2011-10-01
//!
//! 	     Простой класс который помогает засечь время выполнения исходника.
//!
//!          Simple class that helps determining the time of the execution of a source.
//!


#ifndef TIMER_H
#define TIMER_H

#include "includes.h"
#include <assert.h>

/*! \class  Timer Timer.h "Timer.h"
*   \brief  Класс Timer.\n
*           Считает время выполнения исходника с погрешностью ± 1 ms.\n
*           This is a Timer class.\n
*           Сalculates the time of the execution of a source with calculating error ± 1 ms.
*/

class Timer
{
    public:

        Timer();
        ~Timer();
        void start();
        void stop();
        double GetElapsedTime(const char format[64] = "ms");
        bool Ok();

    private:
        LARGE_INTEGER startCount;
        LARGE_INTEGER endCount;
        LARGE_INTEGER frequency;

        double startTime;
        double endTime;
        int startChecker;
        int stopChecker;
        int zombie;
};



//{-------------------------------------------------------------------------------------------
//! \fn      Timer::Timer()
//! \brief   Конструктор: вызывается при создании объекта и инициализирует частные переменные.\n
//!          Constructor: called when creating an object, initializes private variables.
//!
//! \param   startTime          Время начала счета в секундах.\n
//!                             Start time in seconds.
//!
//! \param   endTime            Время конца счета в секундах.\n
//!                             Stop time in seconds.
//!
//! \param   startChecker       Проверка на начало таймера.\n
//!                             Checks if the timer has started.
//!
//! \param   stopChecker        Проверка на конец таймера.\n
//!                             Checks if the timer has stopped.
//!
//! \param   startCount         Первое значение количества "тиков" процессора
//!                             с начала работы системы.\n
//!                             First value of processor ticks since the system was started.
//!
//! \param   endCount           Последнее значение количества "тиков" процессора
//!                             с начала работы системы.\n
//!                             Last value of processor ticks since the system was started.
//!
//! \param   frequency          Количество "тиков" в секунду.\n
//!                             Ticks per second.
//!
//}-------------------------------------------------------------------------------------------

#endif // TIMER_H
