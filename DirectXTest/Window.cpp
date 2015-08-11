#include "Builder.h"

void WindowClass::ok ()
{
	DEFAULT_OK_BLOCK
}

WindowClass::WindowClass (int width,
						  int height,
						  bool resizable)
try :
	hwnd_      (nullptr),
	size_      ({ width, height }),
	resizable_ (resizable)
{
	RegisterApplication ();
}
_END_EXCEPTION_HANDLING (CTOR)

WindowClass::~WindowClass ()
{
	BEGIN_EXCEPTION_HANDLING
	END_EXCEPTION_HANDLING (DTOR)
}

void WindowClass::RegisterApplication ()
{
	try
	{
		static bool done = false;
		if (done) return;

		WNDCLASSEX wnd = {};

		wnd.cbSize = sizeof (wnd);
		wnd.style = CS_HREDRAW | CS_VREDRAW;
		wnd.lpfnWndProc = (WNDPROC)WindowCallback;
		//wnd.hIcon = LoadIcon (NULL, IDI_WINLOGO);
		wnd.hCursor = LoadCursor (NULL, IDC_ARROW);
		wnd.hbrBackground = (HBRUSH)GetStockObject (HOLLOW_BRUSH);
		wnd.lpszClassName = APPLICATION_TITLE_W;

		ATOM wndclass = RegisterClassEx (&wnd);
		if (!wndclass)
			_EXC_N (REGISTER_CLASS, 
					"Failed to register Win32 class")
			done = true;
	}
	_END_EXCEPTION_HANDLING (REGISTER_APP)
}

void WindowClass::SetWindowHandle (HWND hwnd)
{
	hwnd_ = hwnd;
}


LRESULT CALLBACK WindowCallback (HWND windowHandle,
								 UINT msg,
								 WPARAM wparam,
								 LPARAM lparam)
{
	try
	{
		switch (msg)
		{
			case WM_CREATE:
			{
				WindowClass* ptr = reinterpret_cast<WindowClass*> (lparam + offsetof (CREATESTRUCT, lpCreateParams));
				ptr->SetWindowHandle (windowHandle);
				SetWindowLong (windowHandle,
							   GWL_USERDATA,
							   reinterpret_cast<LONG> (ptr));
				break;
			}
			/*
			case WM_CLOSE:
				return on::Close (wnd);
				break;

			case WM_DESTROY:
				on::Destroy (wnd, lparam);
				break;

			case WM_CHAR:
				on::Char (wparam, lparam);
				break;

			case WM_SIZE:
				on::Size (wnd, wparam);
				break;
				*/
			default:
				break;

		}
		return DefWindowProc (windowHandle, msg, wparam, lparam);
	}
	_END_EXCEPTION_HANDLING (WIN_CALLBACK)
}
