#include <windows.h>
#include <windowsx.h>

int WinMain (HINSTANCE hInstance,
			 HINSTANCE legacy,
			 LPSTR lpCmdLine,
			 int nCmdShow)
{
	MessageBoxW (GetForegroundWindow (), L"Hello World!", L"DirectXTest", MB_OK);
}