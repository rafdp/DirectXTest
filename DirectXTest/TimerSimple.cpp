#include "Builder.h"

Timer::Timer () :
	startTime (0),
	endTime (0),
	startChecker (0),
	stopChecker (0)

{
	startCount.QuadPart = 0;
	endCount.QuadPart = 0;
	QueryPerformanceFrequency (&frequency);
}

//{-------------------------------------------------------------------------------------------
//! \brief   Деструктор: вызывается при "разрушении" объекта и портит значения
//!          частных переменных с целью не разрешать их использовать после удаления объекта.\n
//!          Destructor: called when destroying an object, spoils private variables so the
//!          user cannot use them after deleting an object.
//}-------------------------------------------------------------------------------------------

Timer::~Timer ()
{
	zombie = 1;

	startTime = 1;
	startChecker = 0;

	endTime = 0;
	stopChecker = 1;

	frequency.QuadPart = 0;

}

//{-------------------------------------------------------------------------------------------
//! \brief   Start: вызывается пользователем, придает переменной startCount
//!          значение текущего количества "тиков" процессора со включения системы.\n
//!          Start: called by user, sets startCount value to the current
//!          quantity of processor ticks since the system was started;
//}-------------------------------------------------------------------------------------------

void Timer::start ()
{
	startChecker = 1;
	stopChecker = 0;
	QueryPerformanceCounter (&startCount);
	startTime = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
}

//{-------------------------------------------------------------------------------------------
//! \brief   Stop: вызывается пользователем, придает переменной endCount
//!          значение текущего количества "тиков" процессора со включения системы.\n
//!          Stop: called by user, sets endCount value to the current
//!          quantity of processor ticks since the system was started.
//}-------------------------------------------------------------------------------------------

void Timer::stop ()
{
	stopChecker = 1;
	QueryPerformanceCounter (&endCount);
	endTime = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
}

//{-------------------------------------------------------------------------------------------

//! \brief   GetElapsedTime: вызывается пользователем.\n
//!          GetElapsedTime: called by user.
//!
//! \param   format             Формат вывода времени: секунды("s") , миллисекунды("ms")
//!                             или микросекунды ("mks").\n
//!                             Format of the elapsed time: seconds("s") , milliseconds("ms").
//! \return  Возвращает отщитанное время с startTime до endTime.\n
//!          Returns the calculated time from startTime to endTime.
//}-------------------------------------------------------------------------------------------

double Timer::GetElapsedTime (const char format[64])
{
	if (startChecker == 0)
	{
		QueryPerformanceCounter (&startCount);
		startTime = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
	}

	if (stopChecker == 0)
	{
		QueryPerformanceCounter (&endCount);
		endTime = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
	}

	assert (_stricmp (format, "s") == 0 ||
			_stricmp (format, "ms") == 0 ||
			_stricmp (format, "mks") == 0);
	if (_stricmp (format, "s") == 0) return ((endTime - startTime) * 0.000001);
	else if (_stricmp (format, "ms") == 0) return ((endTime - startTime) * 0.001);
	else if (_stricmp (format, "mks") == 0) return (endTime - startTime);
	else return -1;
}


//{-------------------------------------------------------------------------------------------
//! \brief   Ok: вызывается пользователем, проверяет если объект удален или нет.\n
//!          Ok: called by user, checks if the object is deleted or not.
//! \return  Возвращает true если все нормально, false если объект удален.\n
//!          Returns true if everything is ok, false if the object is deleted.
//}-------------------------------------------------------------------------------------------


bool Timer::Ok ()
{
	if (zombie == 1)
		return false;
	else
		return true;
}
