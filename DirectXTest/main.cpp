#include "Builder.h"

ExceptionData_t* __EXPN__ = nullptr;

int WINAPI WinMain (HINSTANCE hInstance,
					HINSTANCE legacy,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	__EXPN__ = new ExceptionData_t (20, "ExceptionErrors.txt");

	try
	{
		//CleverMessageBox ("Hello World!");
		WindowClass window (800, 600);
		Direct3DProcessor d3dProc (&window);

		MSG msg = { 0 };
		while (true)
		{
			if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);

				if (msg.message == WM_QUIT) break;
			}
			// SCENE PROCESSING
			d3dProc.ProcessDrawing ();
			d3dProc.Present ();
		}

	}
	catch (ExceptionHandler_t& ex)
	{
		printf ("Exception occurred\nCheck \"ExceptionErrors.txt\"");
		ex.WriteLog (__EXPN__);
	}
	catch (std::exception err)
	{
		printf ("Exception occurred: %s\n", err.what ());
	}
	catch (...)
	{
		printf ("Exception occurred\n");
	}

	delete __EXPN__;
}