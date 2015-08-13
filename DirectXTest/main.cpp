#include "Builder.h"

ExceptionData_t* __EXPN__ = nullptr;

Direct3DObject* GetCube (Direct3DProcessor* proc);
void GetParticles (Direct3DProcessor* proc);

int WINAPI WinMain (HINSTANCE hInstance,
					HINSTANCE legacy,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	__EXPN__ = new ExceptionData_t (20, "ExceptionErrors.txt");

	srand (time (NULL));

	try
	{
		WindowClass window (1200, 800);
		Direct3DProcessor d3dProc (&window);
		Direct3DCamera cam (&window,
							0.0f, 3.0f, 8.0f,
							0.0f, 0.0f, 0.0f,
							0.0, 1.0f, 0.0f);

		GetParticles (&d3dProc);

		d3dProc.ProcessObjects ();
		//d3dProc.LockShader ();
		cam.Update ();

		MSG msg = {};
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

	Direct3DObject* cube = new Direct3DObject (world, 
											   true, true, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
		{ -1.0f, -1.0f, -1.0f }  //23
	};

	for (int i = 0; i < sizeof (vertices) / sizeof (Vertex_t); i++)
		vertices[i].SetColor (rand () * 1.0f / RAND_MAX, 
							  rand () * 1.0f / RAND_MAX, 
							  rand () * 1.0f / RAND_MAX,
							  0.3f);

	cube->AddVertexArray (vertices, sizeof (vertices) / sizeof (Vertex_t));


	UINT mapping[] =
	{
		0, 3, 1,
		0, 2, 3,

		4, 5, 7,
		4, 7, 6,

		8, 9, 11,
		8, 11, 10,

		12, 14, 15,
		12, 15, 13,

		19, 17, 16,
		18, 19, 16,

		21, 23, 20,
		23, 22, 20
	};

	cube->AddIndexArray (mapping, sizeof (mapping) / sizeof (UINT));
	
	ShaderDesc_t vertS = proc->LoadShader ("shaders.hlsl",
										   "VShader",
										   SHADER_VERTEX);

	cube->AttachVertexShader (vertS);

	cube->AttachPixelShader (proc->LoadShader ("shaders.hlsl",
											   "PShader",
											   SHADER_PIXEL));

	UINT n = proc->AddLayout (vertS, true, false, false, true);

	proc->EnableLayout (n);
	proc->SetLayout (cube, n);

	proc->RegisterObject (cube);

	proc->ApplyBlendState (proc->AddBlendState (true));
	return cube;
}

void GetParticles (Direct3DProcessor* proc)
{
	XMMATRIX world = XMMatrixTranslation (0.0f, 0.0f, 0.0f);

	Vertex_t vertices[3] = {};
	UINT indices[3] = { 0, 1, 2 };
	float x = 0.0f;
	float y = 0.0f;
	ShaderDesc_t vertS = proc->LoadShader ("shaders.hlsl",
										   "VShader",
										   SHADER_VERTEX);

	ShaderDesc_t pixS = proc->LoadShader ("shaders.hlsl",
										   "PShader",
										   SHADER_PIXEL);

	UINT n = proc->AddLayout (vertS, true, false, false, true);

	proc->EnableLayout (n);
	float d = 0;

	for (int i = 0; i < 30000; i++)
	{
		x = (rand () % 1000 - 500.0f) / 150.0f;
		y = (rand () % 1000 - 500.0f) / 150.0f;

		d = rand () * 0.05f / RAND_MAX;

		vertices[0].SetPos (x - d, y - d, 0.0f);
		vertices[0].SetColor (rand () * 1.0f / RAND_MAX,
							  rand () * 1.0f / RAND_MAX,
							  rand () * 1.0f / RAND_MAX, 
							  0.3f);

		vertices[1] = vertices[0];
		vertices[1].y = y + d;

		vertices[2] = vertices[1];
		vertices[2].x = x + d;

		Direct3DObject* particle = new Direct3DObject (world, true, true);
		particle->AddVertexArray (vertices, 3);
		particle->AddIndexArray (indices, 3);

		particle->AttachVertexShader (vertS);
		particle->AttachPixelShader (pixS);
		proc->SetLayout (particle, n);
		proc->RegisterObject (particle);
	}

	proc->ApplyBlendState (proc->AddBlendState (true));
}