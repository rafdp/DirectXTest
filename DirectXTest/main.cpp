#include "Builder.h"

int WINAPI WinMain (HINSTANCE hInstance,
					HINSTANCE legacy,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	MessageBoxW (GetForegroundWindow (), L"Hello World!", L"DirectXTest", MB_OK);
}