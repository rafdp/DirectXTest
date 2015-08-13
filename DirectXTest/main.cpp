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
		WindowClass window (1200, 800);
		Direct3DProcessor d3dProc (&window);
		Direct3DCamera cam (&window,
							4.0f, 4.0f, 6.0f,
							0.0f, 0.0f, 0.0f,
							0.0, 1.0f, 0.0f);

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
			d3dProc.ProcessDrawing (&cam);
			d3dProc.Present ();
		}

	}
	catch (ExceptionHandler_t& ex)
	{
		CleverMessageBox ("Exception occurred\nCheck \"ExceptionErrors.txt\"");
		ex.WriteLog (__EXPN__);
		system ("start ExceptionErrors.txt");
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