#include "Builder.h"

std::string& CreateStringOnFail (const char* text, ...)
{
	std::string* strPtr = new std::string;
	if (!text)
		return *strPtr;

	char str[MAX_STRING_LENGTH] = "";

	va_list arguments;
	va_start (arguments, text);

	vsnprintf (str, MAX_STRING_LENGTH - 2, text, arguments);

	va_end (arguments);

	*strPtr += str;

	return *strPtr;
}

void _MessageBox (const char* text, ...)
{
	if (!text)
	{
		MessageBoxA (GetForegroundWindow (),
					 "Null string",
					 APPLICATION_TITLE_A,
					 MB_OK);
		return;
	}

	char str[MAX_STRING_LENGTH] = "";

	va_list arguments;
	va_start (arguments, text);

	vsnprintf (str, MAX_STRING_LENGTH - 2, text, arguments);

	va_end (arguments);

	MessageBoxA (GetForegroundWindow (),
				 str,
				 APPLICATION_TITLE_A,
				 MB_OK);

}
