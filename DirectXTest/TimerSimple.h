//!
//! \file    TimerSimple.h
//! \brief   Timer class.
//!
//! \version [1.1 Simple]
//! \author  Copyright (C) Rafdp (Raffaele Della Pietra)
//! \date    2011-10-01
//!
//! 	     ������� ����� ������� �������� ������ ����� ���������� ���������.
//!
//!          Simple class that helps determining the time of the execution of a source.
//!


#ifndef TIMER_H
#define TIMER_H

#include "includes.h"
#include <assert.h>

/*! \class  Timer Timer.h "Timer.h"
*   \brief  ����� Timer.\n
*           ������� ����� ���������� ��������� � ������������ � 1 ms.\n
*           This is a Timer class.\n
*           �alculates the time of the execution of a source with calculating error � 1 ms.
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
//! \brief   �����������: ���������� ��� �������� ������� � �������������� ������� ����������.\n
//!          Constructor: called when creating an object, initializes private variables.
//!
//! \param   startTime          ����� ������ ����� � ��������.\n
//!                             Start time in seconds.
//!
//! \param   endTime            ����� ����� ����� � ��������.\n
//!                             Stop time in seconds.
//!
//! \param   startChecker       �������� �� ������ �������.\n
//!                             Checks if the timer has started.
//!
//! \param   stopChecker        �������� �� ����� �������.\n
//!                             Checks if the timer has stopped.
//!
//! \param   startCount         ������ �������� ���������� "�����" ����������
//!                             � ������ ������ �������.\n
//!                             First value of processor ticks since the system was started.
//!
//! \param   endCount           ��������� �������� ���������� "�����" ����������
//!                             � ������ ������ �������.\n
//!                             Last value of processor ticks since the system was started.
//!
//! \param   frequency          ���������� "�����" � �������.\n
//!                             Ticks per second.
//!
//}-------------------------------------------------------------------------------------------

#endif // TIMER_H
