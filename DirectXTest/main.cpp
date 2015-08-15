#include "Builder.h"

ExceptionData_t* __EXPN__ = nullptr;

Direct3DObject* GetCube (Direct3DProcessor* proc);

void ProcessShaderData (Ray* ray);



int WINAPI WinMain (HINSTANCE hInstance,
					HINSTANCE legacy,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	__EXPN__ = new ExceptionData_t (20, "ExceptionErrors.txt");
	srand (static_cast<UINT>(time (NULL)));
	AllocConsole ();
	FILE* file = nullptr;
	freopen_s (&file, "CONOUT$", "w", stdout);
	//printf ("Hello world");
	double time = 0.0;
	int frames = 0;
	Timer t;

	try
	{
		printf ("Loading particles...\n");


		WindowClass window (SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.75f);
		Direct3DProcessor d3dProc (&window);


		ShaderIndex_t vertS = d3dProc.LoadShader ("shaders.fx",
												  "VShader",
												  SHADER_VERTEX);

		ShaderIndex_t geoS = d3dProc.LoadShader ("shaders.fx",
												 "GShader",
												 SHADER_GEOMETRY);

		ShaderIndex_t pixS = d3dProc.LoadShader ("shaders.fx",
												 "PShader",
												 SHADER_PIXEL);
		ParticleSystem ps (-1.0f, 1.0f,
						   -1.0f, 1.0f,
						   -1.0f, 1.0f,
						   100000,
						   0.0f, 0.75f, 1.0f, 0.05f,
						   0.01f);
		printf ("Particles loaded\n");
		XMFLOAT4 camPos = { 0.0f, 3.0f, 8.0f, 1.0f };
		Direct3DCamera cam (&window,
							camPos.x, camPos.y, camPos.z,
							0.0f, 0.0f, 0.0f,
							0.0, 1.0f, 0.0f,
							0.15f);

		UINT camBuf = d3dProc.RegisterConstantBuffer (&camPos, sizeof (camPos), 1);
		d3dProc.UpdateConstantBuffer (camBuf);

		float x_ = rand () * 1.0f / RAND_MAX;
		float y_ = rand () * 1.0f / RAND_MAX;
		float z_ = rand () * 1.0f / RAND_MAX;
		
		Ray ray (2,
				 &d3dProc,
				 { 1.0f, 0.0f, 0.0f, 0.9f },
				 { x_, y_, z_, 1.0f },
				 { 0.1f - x_, -0.3f - y_, 0.5f - z_, 0.0f },
				 0.1f, 1.0f, 1.0f);

		Direct3DObject* obj = GetCube (&d3dProc);

		XMMATRIX world = XMMatrixTranslation (0.0f, 0.0f, 0.0f);


		Direct3DObject* particles = new (GetValidObjectPtr ()) 
				Direct3DObject (world, false, false, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		d3dProc.ApplyBlendState (d3dProc.AddBlendState (true));

		

		ps.DumpToObject (particles);
		d3dProc.RegisterObject (particles);

		d3dProc.AttachShaderToObject (particles, vertS);
		d3dProc.AttachShaderToObject (particles, pixS);
		d3dProc.AttachShaderToObject (particles, geoS);


		d3dProc.EnableLayout (d3dProc.AddLayout (vertS, true, false, false, true));

		d3dProc.ProcessObjects ();
		cam.Update ();

		
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
			ProcessShaderData (&ray);

			obj->GetWorld () = particles->GetWorld () *= XMMatrixRotationX (0.005f) * XMMatrixRotationY (0.01f) * XMMatrixRotationZ (0.015f);
			
			d3dProc.SendCBToGS (camBuf);
			ray.SendToGS ();
			d3dProc.ProcessDrawing (&cam);
			d3dProc.Present ();
			t.stop ();
			frames++;
			time += t.GetElapsedTime ("mks");
			if (time >= 500000.0)
			{
				printf ("\r%f %d     ", ray.pow, frames * 2);
				time = 0.0;
				frames = 0;
			}
			getchar ();
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
														"PShader",
														SHADER_PIXEL));

	UINT n = proc->AddLayout (vertS, true, false, false, true);

	proc->EnableLayout (n);
	proc->SetLayout (cube, n);

	proc->RegisterObject (cube);

	proc->ApplyBlendState (proc->AddBlendState (true));
	return cube;
}

void ProcessShaderData (Ray* ray)
{
	if (GetAsyncKeyState (VK_UP) & 0x8000)
		if (ray->pow < 10.0f) ray->pow += 0.1f;
	if (GetAsyncKeyState (VK_DOWN) & 0x8000)
		if (ray->pow > -1.0f) ray->pow -= 0.1f;
	for (int8_t i = 0; i < 9; i++)
		if (GetAsyncKeyState ('0' + i)) ray->pow = i;
	if (GetAsyncKeyState (VK_RETURN) & 0x8000)
	{
		ray->Update ();
		while (GetAsyncKeyState (VK_RETURN));
	}
}