#include "Builder.h"

void Raytracer::ok ()
{
	DEFAULT_OK_BLOCK
	if (ps_ == nullptr)
		_EXC_N (NULL_PARTICLE_SYSTEM_PTR, "Raytracer: Null ParticleSystem pointer")
}

Raytracer::Raytracer (ParticleSystem* ps,
					  UINT cbSlot,
					  XMFLOAT4 pointColor,
					  XMFLOAT4 rayColor,
					  float cosPow,
					  float pointsScale,
					  float range,
					  float cosClipping,
					  XMFLOAT3 rayStartPos,
					  XMFLOAT3 rayStartDir)
try :
	ps_              (ps),
	rayData_         ({pointColor, rayColor, {cosPow, 
											  pointsScale, 
											  cosClipping,
											  range}}),
	rayStartPos_     (rayStartPos),
	rayStartDir_     (rayStartDir),
	nConstantBuffer_ (),
	refract_         (0.0f, 60.0f, 1.0f),
	stop_            (false),
	ready_           (true)
{
	ok ();
	nConstantBuffer_ = ps_->proc_->RegisterConstantBuffer (&rayData_, 
													  sizeof (rayData_), 
													  cbSlot);
}
_END_EXCEPTION_HANDLING (CTOR)

Raytracer::~Raytracer ()
{
	ps_ = nullptr;
	rayData_ = {};
	rayStartPos_ = {};
	rayStartDir_ = {};
}

DWORD WINAPI ThreadedRaytracing (void* ptr)
{
	ThreadData_t& data = *reinterpret_cast<ThreadData_t*> (ptr);
	ThreadData_t copy = data;
	data.read = true;
	XMVECTOR curPos = *copy.currentPos;
	float distance = 0.0f;
	for (uint64_t i = 0; i < copy.size; i++)
	{
		XMVECTOR d = XMVector3Length (XMVectorSet (copy.data[i].r,
												   copy.data[i].g,
												   copy.data[i].b,
												   0.0f) - curPos);
		XMStoreFloat (&distance, d);
		if (distance < copy.range &&
			(distance < copy.data[i].a ||
			 copy.data[i].a < -0.5f))
			 copy.data[i].a = distance;
	}
	data.done++;
	return 0;
}

void Raytracer::TraceRay ()
{
	BEGIN_EXCEPTION_HANDLING
	if (!ready_) stop_ = true;
	while (!ready_);
	ready_ = false;
	CreateThread (NULL,
				  0,
				  ParallelProcessing,
				  this,
				  0,
			      NULL);
	//while (!ready_);

	END_EXCEPTION_HANDLING (TRACE_RAY)
}

void Raytracer::Stop ()
{
	BEGIN_EXCEPTION_HANDLING
	if (!ready_) stop_ = true;
	while (!ready_);

	END_EXCEPTION_HANDLING (STOP)
}

void Raytracer::SetDrawRayOnly ()
{
	rayData_.rayOnly = fabs (rayData_.rayOnly);
}
void Raytracer::SetDrawAll ()
{
	rayData_.rayOnly = -fabs (rayData_.rayOnly);
}
void Raytracer::Update ()
{
	ps_->proc_->UpdateConstantBuffer (nConstantBuffer_);
}
void Raytracer::SendToGS ()
{
	ps_->proc_->SendCBToGS (nConstantBuffer_);
}
void Raytracer::SendToVS ()
{
	ps_->proc_->SendCBToVS (nConstantBuffer_);
}
void Raytracer::PrepareToDraw0 ()
{
	BEGIN_EXCEPTION_HANDLING
	SetDrawRayOnly ();
	Update ();
	SendToVS ();
#ifndef ENHANCE_PERFORMANCE
	ps_->PrepareToDraw ();
#endif
	END_EXCEPTION_HANDLING (PREPARE_TO_DRAW0)
}
void Raytracer::PrepareToDraw1 ()
{
	BEGIN_EXCEPTION_HANDLING
	SetDrawAll ();
	Update ();
	SendToVS ();
#ifndef ENHANCE_PERFORMANCE
	ps_->PrepareToDraw ();
#endif
	END_EXCEPTION_HANDLING (PREPARE_TO_DRAW1)
}

float& Raytracer::GetCosPow ()
{
	return rayData_.cosPow;
}

DWORD WINAPI Raytracer::ParallelProcessing (void* ptr)
{
	Raytracer& _this = *reinterpret_cast<Raytracer*> (ptr);
	try
	{
		_this.ok ();
		UINT iterations = 0;
		XMVECTOR currentPos = XMLoadFloat3 (&_this.rayStartPos_);
		XMVECTOR currentDir = XMVector3Normalize (XMLoadFloat3 (&_this.rayStartDir_));
		float currentN = 1.0f;
		XMFLOAT2 distance = {};
		std::vector<Vertex_t>& particles = _this.ps_->particles_;
		
		/*for (auto i = particles.begin ();
		i < particles.end ();
			i++)
			i->a = -1.0f;*/

		float step = float (_this.rayData_.range / 3.0 * fabs (_this.rayData_.rayOnly));

		while (true)
		{
			XMVECTOR d = XMVector3Length (currentPos);
			XMStoreFloat2 (&distance, currentPos);
			if (distance.y < 1.0f && distance.y > -1.0f) break;

			currentPos += currentDir * step;
			iterations++;

			if (iterations > 1000)
			{
				_MessageBox ("Raytracer: The ray didn't intersect the object");
				return 0;
			}
		}
		if (_this.stop_) { _this.stop_ = false; return 0; }
		iterations = 0;

		XMVECTOR currentPosBak = currentPos;
		XMVECTOR currentDirBak = currentDir;
		float currentNbak = currentN;
		while (true)
		{

			XMStoreFloat2 (&distance, currentPosBak);
			if (distance.y > 1.0f || distance.y < -1.0f) break;
			iterations++;
			_this.refract_.Process (currentPosBak, currentDirBak, currentNbak);

			currentPosBak += currentDirBak * step;
			if (iterations > 1000)
				break;
		}
		if (_this.stop_) { _this.stop_ = false; return 0; }

		UINT Niterations = iterations;
		iterations = 0;

		size_t size = particles.size ();
		UINT threadsN = std::thread::hardware_concurrency ();
		uint64_t sizeSingle = size / threadsN;
		uint64_t shift = 0;

		ThreadData_t data (&currentPos, _this.rayData_.range);

		while (true)
		{
			if (_this.stop_) { _this.stop_ = false; return 0; }
			PrintfProgressBar (iterations, Niterations);

			XMStoreFloat2 (&distance, currentPos);
			if (distance.y > 1.0f || distance.y < -1.0f) break;
			data.done = 0;
			//_putch ('C');
			//thread stuff
			{
				for (uint8_t i = 0; i < threadsN - 1; i++)
				{
					data.data = particles.data () + shift;
					data.size = sizeSingle;
					data.read = false;

					CreateThread (NULL,
								  0,
								  ThreadedRaytracing,
								  &data,
								  0,
								  NULL);

					while (!data.read);
					shift += sizeSingle;
				}
				data.data = particles.data () + shift;
				data.size = size - shift;
				CreateThread (NULL,
							  0,
							  ThreadedRaytracing,
							  &data,
							  0,
							  NULL);
			}

			while (data.done < threadsN);

			_this.refract_.Process (currentPos, currentDir, currentN);

			currentPos += currentDir * step;
			iterations++;

			shift = 0;
			if (iterations > 1000)
				break;
		}

		_this.ready_ = true;
		_putch ('\n');
		_this.ps_->object_->EnterCriticalSection ();
		_this.ps_->object_->ClearVertexArray ();
		_this.ps_->object_->AddVertexArray (_this.ps_->particles_.data (),
											_this.ps_->particles_.size ());
		_this.ps_->object_->SetVertexBuffer (_this.ps_->proc_->GetDevice ());
		_this.ps_->object_->ExitCriticalSection ();
		_this.ok ();
	}
	catch (ExceptionHandler_t& ex)
	{
		_MessageBox ("Exception occurred (WINDOW_CALLBACK)\nCheck \"ExceptionErrors.txt\"");
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
	return 0;
}


void Raytracer::SetCosPow (float setting)
{
	if (setting > -1.0f && setting <= 20.1f)
		rayData_.cosPow = setting;
}
void Raytracer::SetRange (float setting)
{
	if (setting > 0.0f && setting < 1.0f)
		rayData_.range = setting;
}
void Raytracer::SetScale (float setting)
{
	if (setting > 0.0f && setting < 10.0f)
		rayData_.pointsScale = setting;
}
void Raytracer::SetCosClip (float setting)
{
	if (setting > 0.0f && setting < 1.0f)
		rayData_.rayOnly *= setting / fabs (rayData_.rayOnly);
}
void Raytracer::SetRayColor (XMFLOAT4 color)
{
	XMStoreFloat4 (&rayData_.rayColor,
				   XMVectorSaturate (XMLoadFloat4 (&color)));
}
void Raytracer::SetRayColorR (float setting)
{
	if (setting > 1.0f) rayData_.rayColor.x = 1.0f;
	else
	if (setting < 0.0f) rayData_.rayColor.x = 0.0f;
	else
		rayData_.rayColor.x = setting;
}
void Raytracer::SetRayColorG (float setting)
{
	if (setting > 1.0f) rayData_.rayColor.y = 1.0f;
	else
	if (setting < 0.0f) rayData_.rayColor.y = 0.0f;
	else
		rayData_.rayColor.y = setting;
}
void Raytracer::SetRayColorB (float setting)
{
	if (setting > 1.0f) rayData_.rayColor.z = 1.0f;
	else
	if (setting < 0.0f) rayData_.rayColor.z = 0.0f;
	else
		rayData_.rayColor.z = setting;
}
void Raytracer::SetRayColorA (float setting)
{
	if (setting > 1.0f) rayData_.rayColor.w = 1.0f;
	else
	if (setting < 0.0f) rayData_.rayColor.w = 0.0f;
	else
		rayData_.rayColor.w = setting;
}
void Raytracer::SetTopTemp (float setting)
{
	refract_.SetTopTemperature (setting);
}
void Raytracer::SetBottomTemp (float setting)
{
	refract_.SetBottomTemperature (setting);
}
void Raytracer::SetTransition (float setting)
{
	refract_.SetTransitionLength (setting);
}
void Raytracer::Join ()
{
	if (!ready_)
		while (!ready_);
}

void Raytracer::Clear ()
{
	for (auto i = ps_->particles_.begin ();
	i < ps_->particles_.end ();
		i++)
		i->a = -1.0f;
	ps_->object_->EnterCriticalSection ();
	ps_->object_->ClearVertexArray ();
	ps_->object_->AddVertexArray (ps_->particles_.data (),
		   						  ps_->particles_.size ());
	ps_->object_->SetVertexBuffer (ps_->proc_->GetDevice ());
	ps_->object_->ExitCriticalSection ();
}

void ScriptCompiler::ok ()
{
	DEFAULT_OK_BLOCK
	if (!rt_)
		_EXC_N (NULL_RAYTRACER_PTR, "ScriptCompiler: Null Raytracer ptr")
}


ScriptCompiler::ScriptCompiler (std::string filename,
								Raytracer* rt)
	try :
	filename_ (filename),
	rt_       (rt)
{}
_END_EXCEPTION_HANDLING (CTOR)
ScriptCompiler::~ScriptCompiler ()
{
	filename_.clear ();
	rt_ = nullptr;
}

void ScriptCompiler::Run ()
{
	BEGIN_EXCEPTION_HANDLING
	FILE* f = nullptr;
	fopen_s (&f, filename_.c_str (), "r");

	if (!f)
	{
		_MessageBox ("Failed to open script \"%s\"", filename_.c_str ());
		return;
	}
	std::string current;
	char buffer[4096] = "";
	uint16_t index = 0;
	float arg = 0.0f;
	uint32_t line = 0;
	while (!feof (f))
	{
		index = 0;
		current.clear ();
		arg = 0.0f;
		fgets (buffer, 4096, f);
		while (buffer[index] == ' ' && buffer[index] && buffer[index] != '\n') index++;
		if (!buffer[index] || buffer[index] == '\n') continue;
		while (buffer[index] != ' ' && buffer[index] && buffer[index] != '\n')
		{
			current += buffer[index];
			index++;
		}

		if (current == "Render")
		{
			rt_->TraceRay ();
			continue;
		}
		if (current == "Join")
		{
			rt_->Join ();
			continue;
		}
		if (current == "Clear")
		{
			rt_->Join ();
			continue;
		}

		while (buffer[index] != ' ' && buffer[index] && buffer[index] != '\n')
		{
			current += buffer[index];
			index++;
		}
		
		while (buffer[index] == ' ' && buffer[index] && buffer[index] != '\n') index++;
		char c = *(buffer + index);
		if ((c > '9' || c < '0') && c != '+' && c != '-')
		{
			_MessageBox ("Invalid character:\n%s\n\index %d \'%c\'", 
						 buffer, 
						 c, 
						 index);
			return;
		}
		arg = float (atof (buffer + index));

#define CHECK_CURRENT(name) if (current == #name) { rt_->##name (arg); rt_->Update (); continue;}
#define CHECK_CURRENT_(name) if (current == #name) { rt_->##name (arg);  continue;}

		//_MessageBox ("%u", line);
		CHECK_CURRENT_ (SetTopTemp)
		CHECK_CURRENT_ (SetBottomTemp)
		CHECK_CURRENT_ (SetTransition)
		CHECK_CURRENT (SetRayColorR)
		CHECK_CURRENT (SetRayColorG)
		CHECK_CURRENT (SetRayColorB)
		CHECK_CURRENT (SetRayColorA)
		CHECK_CURRENT (SetRange)
		CHECK_CURRENT (SetCosPow)
		CHECK_CURRENT (SetCosClip)
#undef CHECK_CURRENT_ 
#undef CHECK_CURRENT

		_MessageBox ("Found illegal line:\n%s", buffer);

	}

	fclose (f);
	END_EXCEPTION_HANDLING (RUN)
}

ThreadData_t::ThreadData_t (const ThreadData_t & that) : 
	currentPos (that.currentPos),
	range      (that.range),
	data       (that.data),
	size       (that.size)
{}

ThreadData_t::ThreadData_t (XMVECTOR * currentPos_, float range_) : 
	currentPos (currentPos_),
	range      (range_),
	data       (),
	size       (),
	read       (),
	done       ()
{}
