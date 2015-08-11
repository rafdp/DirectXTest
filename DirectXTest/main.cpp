#include "Builder.h"

ExceptionData_t* __EXPN__ = nullptr;

int WINAPI WinMain (HINSTANCE hInstance,
					HINSTANCE legacy,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	__EXPN__ = new ExceptionData_t (20, "ExceptionErrors.txt");

	MessageBoxW (GetForegroundWindow (), L"Hello World!", APPLICATION_TITLE_W, MB_OK);

	delete __EXPN__;
}