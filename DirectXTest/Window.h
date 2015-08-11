#pragma once

#include "includes.h"

class WindowClass : NZA_t
{
private:
	HWND hwnd_;
	SIZE size_;
	bool resizable_;

	void SetWindowHandle (HWND hwnd);

public:
	void ok ();
	WindowClass (int widthC, 
				 int heightC, 
				 bool resizable = false);
	~WindowClass ();

	static void RegisterApplication ();

	friend LRESULT CALLBACK WindowCallback (HWND windowHandle,
											UINT msg,
											WPARAM wparam,
											LPARAM lparam);
};

void LinkHWNDToClass (HWND wnd, LPARAM& createStr);

LRESULT CALLBACK WindowCallback (HWND windowHandle, 
								 UINT msg, 
								 WPARAM wparam, 
								 LPARAM lparam);