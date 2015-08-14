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
//! \brief   ����������: ���������� ��� "����������" ������� � ������ ��������
//!          ������� ���������� � ����� �� ��������� �� ������������ ����� �������� �������.\n
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
//! \brief   Start: ���������� �������������, ������� ���������� startCount
//!          �������� �������� ���������� "�����" ���������� �� ��������� �������.\n
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
//! \brief   Stop: ���������� �������������, ������� ���������� endCount
//!          �������� �������� ���������� "�����" ���������� �� ��������� �������.\n
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

//! \brief   GetElapsedTime: ���������� �������������.\n
//!          GetElapsedTime: called by user.
//!
//! \param   format             ������ ������ �������: �������("s") , ������������("ms")
//!                             ��� ������������ ("mks").\n
//!                             Format of the elapsed time: seconds("s") , milliseconds("ms").
//! \return  ���������� ���������� ����� � startTime �� endTime.\n
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
//! \brief   Ok: ���������� �������������, ��������� ���� ������ ������ ��� ���.\n
//!          Ok: called by user, checks if the object is deleted or not.
//! \return  ���������� true ���� ��� ���������, false ���� ������ ������.\n
//!          Returns true if everything is ok, false if the object is deleted.
//}-------------------------------------------------------------------------------------------


bool Timer::Ok ()
{
	if (zombie == 1)
		return false;
	else
		return true;
}
