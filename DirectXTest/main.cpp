#include "Builder.h"

ExceptionData_t* __EXPN__ = nullptr;

Direct3DObject* GetCube (Direct3DProcessor* proc);
void GetParticles (Direct3DProcessor* proc);
Direct3DObject* GetParticles_ (Direct3DProcessor* proc);


struct CBData
{
	XMFLOAT4 color;
	XMFLOAT4 point;
	XMFLOAT4 vector;
	float min;
	float pow;
	float pad[2];
}; 

void ProcessShaderData (CBData* data, Direct3DProcessor* proc, UINT n);



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
		ParticleSystem ps (-1.0f, 1.0f,
						   -1.0f, 1.0f,
						   -1.0f, 1.0f,
						   300000, 0.8f,
						   0.0f, 0.75f, 1.0f, 0.2f,
						   0.01f);
		printf ("Particles loaded\n");
		WindowClass window (1800, 1200);
		//SetStdHandle (STD_OUTPUT_HANDLE, );
		Direct3DProcessor d3dProc (&window);
		Direct3DCamera cam (&window,
							0.0f, 3.0f, 8.0f,
							0.0f, 0.0f, 0.0f,
							0.0, 1.0f, 0.0f,
							0.15f);

		float x_ = rand () * 1.0f / RAND_MAX;
		float y_ = rand () * 1.0f / RAND_MAX;
		float z_ = rand () * 1.0f / RAND_MAX;

		CBData passToShader =
		{
			{ 1.0f, 0.0f, 0.0f, 1.0f },
			{ x_, y_, z_, 1.0f },
			{ 0.1f - x_, -0.3f - y_, 0.5f - z_, 0.0f },
			0.1f, 1.0f
		};
		Direct3DObject* obj = GetCube (&d3dProc);

		UINT cbN = d3dProc.RegisterConstantBuffer (&passToShader, 
												   sizeof (CBData), 
												   8);
		d3dProc.UpdateConstantBuffer (cbN);

		XMMATRIX world = XMMatrixTranslation (0.0f, 0.0f, 0.0f);


		Direct3DObject* particles = new (GetValidObjectPtr ()) Direct3DObject (world, true);

		d3dProc.ApplyBlendState (d3dProc.AddBlendState (true));

		/*ps.ApplyRay (1.0f, 0.0f, 0.0f, 
					 3.0f, 4.0f, 3.0f,
					 -3.0f, -4.0f, -2.25f,
					 0.05f);*/

		ps.DumpToObject (particles);
		d3dProc.RegisterObject (particles);

		ShaderDesc_t vertS = d3dProc.LoadShader ("shaders.hlsl",
											   "VShader",
											   SHADER_VERTEX);

		ShaderDesc_t pixS = d3dProc.LoadShader ("shaders.hlsl",
											  "PShader",
											  SHADER_PIXEL);

		particles->AttachVertexShader (vertS);
		particles->AttachPixelShader (pixS);


		d3dProc.EnableLayout (d3dProc.AddLayout (vertS, true, false, false, true));

		/*Direct3DObject* obj = GetParticles_ (&d3dProc);
		Direct3DObject* cube = GetCube (&d3dProc);*/

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
			ProcessShaderData (&passToShader, &d3dProc, cbN);

			obj->GetWorld () = particles->GetWorld () *= XMMatrixRotationX (0.005f) * XMMatrixRotationY (0.01f) * XMMatrixRotationZ (0.015f);
			d3dProc.SendCBToVS (cbN);
			d3dProc.ProcessDrawing (&cam);
			d3dProc.Present ();
			t.stop ();
			frames++;
			time += t.GetElapsedTime ("mks");
			if (time >= 500000.0)
			{
				printf ("\r%f %d     ", passToShader.pow, frames * 2);
				time = 0.0;
				frames = 0;
			}
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
	
	ShaderDesc_t vertS = proc->LoadShader ("shaders.hlsl",
										   "VShaderCube",
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

	for (int i = 0; i < 200; i++)
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

		Direct3DObject* particle = new (GetValidObjectPtr ()) Direct3DObject (world, true, true);
		particle->AddVertexArray (vertices, 3);
		particle->AddIndexArray (indices, 3);

		particle->AttachVertexShader (vertS);
		particle->AttachPixelShader (pixS);
		proc->SetLayout (particle, n);
		proc->RegisterObject (particle);
	}

	proc->ApplyBlendState (proc->AddBlendState (true));
}


Direct3DObject* GetParticles_ (Direct3DProcessor* proc)
{
	XMMATRIX world = XMMatrixTranslation (0.0f, 0.0f, 0.0f);

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Direct3DObject* particle = new (GetValidObjectPtr ()) Direct3DObject (world, true, false);

	std::vector<Vertex_t> vertices;
	std::vector<Vertex_t> indices;
	ShaderDesc_t vertS = proc->LoadShader ("shaders.hlsl",
										   "VShader",
										   SHADER_VERTEX);

	ShaderDesc_t pixS = proc->LoadShader ("shaders.hlsl",
										  "PShader",
										  SHADER_PIXEL);

	UINT n = proc->AddLayout (vertS, true, false, false, true);

	Vertex_t vertsC[3] = {};

	proc->EnableLayout (n);
	float d = 0;

	for (int i = 0; i < 100000; i++)
	{
		x = (rand () % 1000 - 500.0f) / 500.0f;
		y = (rand () % 1000 - 500.0f) / 500.0f;
		z = (rand () % 1000 - 500.0f) / 500.0f;

		d = rand () * 0.03f / RAND_MAX;

		

		vertsC[0].SetPos (x - d, 
						  y - d, 
						  z);
		vertsC[0].SetColor (0.82f + rand () * 0.01f / RAND_MAX,
							0.94f + rand () * 0.01f / RAND_MAX,
							1.0f - rand () * 0.01f / RAND_MAX,
							0.1f);

		vertsC[1] = vertsC[0];
		vertsC[1].y = y + d;

		vertsC[2] = vertsC[1];
		vertsC[2].x = x + d;

		vertices.push_back (vertsC[0]);
		vertices.push_back (vertsC[1]);
		vertices.push_back (vertsC[2]);

	}
	particle->AddVertexArray (vertices.data (), vertices.size ());
	proc->SetLayout (particle, n);
	proc->RegisterObject (particle);

	particle->AttachVertexShader (vertS);
	particle->AttachPixelShader (pixS);

	proc->ApplyBlendState (proc->AddBlendState (true));
	return particle;
}

void ProcessShaderData (CBData* data, Direct3DProcessor* proc, UINT n)
{
	if (GetAsyncKeyState (VK_UP) & 0x8000)
		if (data->pow < 10.0f) data->pow += 0.1f;
	if (GetAsyncKeyState (VK_DOWN) & 0x8000)
		if (data->pow > -1.0f) data->pow -= 0.1f;
	for (int8_t i = 0; i < 9; i++)
		if (GetAsyncKeyState ('0' + i)) data->pow = i;
	if (GetAsyncKeyState (VK_RETURN) & 0x8000)
	{
		proc->UpdateConstantBuffer (n);
		while (GetAsyncKeyState (VK_RETURN));
	}
}