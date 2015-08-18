
#include "Builder.h"

ExceptionData_t* __EXPN__ = nullptr;

Direct3DObject* GetCube (Direct3DProcessor* proc);

void ProcessShaderData (Raytracer* rt);



int WINAPI WinMain (HINSTANCE hInstance,
					HINSTANCE legacy,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	__EXPN__ = new ExceptionData_t (20, "ExceptionErrors.txt");
	srand (static_cast<UINT>(time (NULL)));
	double time = 0.0;
	int frames = 0;
	Timer t;

	try
	{
		WindowClass window (int (SCREEN_WIDTH * 0.75f), int (SCREEN_HEIGHT * 0.75f));
		AllocConsole ();
		FILE* file = nullptr;
		freopen_s (&file, "CONOUT$", "w", stdout);

		printf ("Loading particles...\n");
		Direct3DProcessor d3dProc (&window);
		d3dProc.ApplyBlendState (d3dProc.AddBlendState (true));

		XMMATRIX world = XMMatrixTranslation (0.0f, 0.0f, 0.0f);


		Direct3DObject* particles = new (GetValidObjectPtr ())
			Direct3DObject (world, false, false, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		float x_ = 0.7f;
		float y_ = 1.5f;
		float z_ = 0.8f;
#ifndef ENHANCE_PERFORMANCE
		ParticleSystem ps (particles, &d3dProc,
						   -1.0f, 1.0f,
						   -1.0f, 1.0f,
						   -1.0f, 1.0f,
						   1000000);
		Raytracer raytracer (&ps, 2,
							 { 0.0f, 0.8f, 1.0f, 0.08f },
							 { 1.0f, 0.0f, 0.0f, 0.8f },
							 4.0f, 1.0f, 0.05f, 0.5f, // cospow, scale, range, clip
							 { x_, y_, z_ },
							 { -0.1f - x_, -0.3f - y_, -0.5f - z_ });
#else
		ParticleSystem ps (particles, &d3dProc,
						   -1.0f, 1.0f,
						   -1.0f, 1.0f,
						   -1.0f, 1.0f,
						   100000);
		Raytracer raytracer (&ps, 2,
							{ 0.0f, 0.8f, 1.0f, 0.075f },
							{ 1.0f, 0.0f, 0.0f, 0.8f },
							 4.0f, 1.0f, 0.05f, 0.5f, // cospow, scale, range, clip
							 { x_, y_, z_ },
							 { 0.1f - x_, -0.3f - y_, 0.5f - z_ });
#endif
		

		

		printf ("Particles loaded\n");
		XMFLOAT4 camPos = { 0.0f, 0.0f, 8.0f, 1.0f };
		Direct3DCamera cam (&window,
							camPos.x, camPos.y, camPos.z,
							0.0f, 0.0f, 0.0f,
							0.0, 1.0f, 0.0f,
							0.15f);

		ConstantBufferIndex_t camBuf = d3dProc.RegisterConstantBuffer (&camPos, 
																	   sizeof (camPos), 
																	   1);
		d3dProc.UpdateConstantBuffer (camBuf);

		Direct3DObject* obj = GetCube (&d3dProc);

		cam.Update ();

		printf ("Tracing ray\n");
		raytracer.TraceRay ();
		printf ("Ray ready\n");
		SetForegroundWindow (window.hwnd ());

		ps.DumpVerticesToObject ();


		d3dProc.ProcessObjects ();

		
		MSG msg = {};
		while (true)
		{
			t.start ();
			if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);

				if (msg.message == WM_QUIT) break;
			}
			// SCENE PROCESSING
			ProcessShaderData (&raytracer);

			if (GetAsyncKeyState (VK_MENU))
			{
				while (GetAsyncKeyState (VK_MENU));
				d3dProc.ReloadShaders ();
				printf ("Shaders reloaded\n");
			}

			obj->GetWorld () = particles->GetWorld () *=  XMMatrixRotationY (0.01f);
			d3dProc.SendCBToGS (camBuf);
			raytracer.PrepareToDraw0 ();
			d3dProc.ProcessDrawing (&cam);

			d3dProc.SendCBToGS (camBuf);
			raytracer.PrepareToDraw1 ();
			d3dProc.ProcessDrawing (&cam, false);
			d3dProc.Present ();

			t.stop ();
			frames++;
			time += t.GetElapsedTime ("mks");
			if (time >= 500000.0)
			{
				printf ("\r%f %d     ", raytracer.GetCosPow (), frames * 2);
				time = 0.0;
				frames = 0;
			}
			//Sleep (100);
		}
		FreeConsole ();
	}
	catch (ExceptionHandler_t& ex)
	{
		_MessageBox ("Exception occurred\nCheck \"ExceptionErrors.txt\"");
		ex.WriteLog (__EXPN__);
		system ("start ExceptionErrors.txt");
	}
	catch (std::exception err)
	{
		_MessageBox ("Exception occurred: %s\n", err.what ());
	}
	catch (...)
	{
		_MessageBox ("Exception occurred\n");
	}

	delete __EXPN__;
	return 0;
}



Direct3DObject* GetCube (Direct3DProcessor* proc)
{
	XMMATRIX world = XMMatrixTranslation (0.0f, 0.0f, 0.0f);

	Direct3DObject* cube = new (GetValidObjectPtr ())
							   Direct3DObject (world,
											   false, 
											   true, 
											   D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	Vertex_t vertices[] =
	{
		{ 1.0f,  1.0f,  1.0f }, //0  left
		{ 1.0f,  1.0f, -1.0f }, //1
		{ 1.0f, -1.0f,  1.0f }, //2
		{ 1.0f, -1.0f, -1.0f }, //3

		{ -1.0f,  1.0f,  1.0f }, //4  right
		{ -1.0f,  1.0f, -1.0f }, //5
		{ -1.0f, -1.0f,  1.0f }, //6
		{ -1.0f, -1.0f, -1.0f }, //7
		/*
		{ 1.0f,  1.0f,  1.0f }, //8  top
		{ 1.0f,  1.0f, -1.0f }, //9
		{ -1.0f,  1.0f,  1.0f }, //10
		{ -1.0f,  1.0f, -1.0f }, //11

		{ 1.0f, -1.0f,  1.0f }, //12 bottom
		{ 1.0f, -1.0f, -1.0f }, //13
		{ -1.0f, -1.0f,  1.0f }, //14
		{ -1.0f, -1.0f, -1.0f }, //15

		{ 1.0f,  1.0f,  1.0f }, //16 front
		{ 1.0f, -1.0f,  1.0f }, //17
		{ -1.0f,  1.0f,  1.0f }, //18
		{ -1.0f, -1.0f,  1.0f }, //19

		{ 1.0f,  1.0f, -1.0f }, //20 back
		{ 1.0f, -1.0f, -1.0f }, //21
		{ -1.0f,  1.0f, -1.0f }, //22
		{ -1.0f, -1.0f, -1.0f }  //23*/
	};

	for (int i = 0; i < sizeof (vertices) / sizeof (Vertex_t); i++)
		vertices[i].SetColor (0.0f, 
							  0.75f, 
							  1.0f,
							  0.5f);

	cube->AddVertexArray (vertices, sizeof (vertices) / sizeof (Vertex_t));


	UINT mapping[] =
	{
		0, 1,
		1, 3, 
		3, 2,
		2, 0,

		4, 5,
		5, 7,
		7, 6,
		6, 4,
		
		0, 4,
		1, 5,
		2, 6,
		3, 7,
		/*
		12, 13,
		13, 14,
		14, 15,
		15, 12,

		16, 17,
		17, 18,
		18, 19,
		19, 16,

		20, 21,
		21, 22,
		22, 23,
		23, 20*/
	};

	cube->AddIndexArray (mapping, sizeof (mapping) / sizeof (UINT));
	
	ShaderIndex_t vertS = proc->LoadShader ("shaders.fx",
										   "VShaderCube",
										   SHADER_VERTEX);
	proc->AttachShaderToObject (cube, vertS);
	proc->AttachShaderToObject (cube, proc->LoadShader ("shaders.fx",
														"PShaderCube",
														SHADER_PIXEL));

	UINT n = proc->AddLayout (vertS, true, false, false, true);

	proc->EnableLayout (n);
	proc->SetLayout (cube, n);

	proc->RegisterObject (cube);

	proc->ApplyBlendState (proc->AddBlendState (true));
	return cube;
}

void ProcessShaderData (Raytracer* rt)
{
	if (GetAsyncKeyState (VK_UP) & 0x8000)
		if (rt->GetCosPow () < 10.0f) rt->GetCosPow () += 0.1f;
	if (GetAsyncKeyState (VK_DOWN) & 0x8000)
		if (rt->GetCosPow () > -1.0f) rt->GetCosPow () -= 0.1f;
	for (int8_t i = 0; i <= 9; i++)
		if (GetAsyncKeyState ('0' + i)) rt->GetCosPow () = i;
	if (GetAsyncKeyState (VK_RETURN) & 0x8000)
	{
		rt->Update ();
		while (GetAsyncKeyState (VK_RETURN));
	}
}