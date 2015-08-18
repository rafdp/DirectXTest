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
	refract_         (0.0f, 90.0f, 1.0f)
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
	UINT iterations = 0;
	XMVECTOR currentPos = XMLoadFloat3 (&rayStartPos_);
	XMVECTOR currentDir = XMVector3Normalize (XMLoadFloat3 (&rayStartDir_));
	float currentN = 1.0f;
	float distance = 0.0f;
	std::vector<Vertex_t>& particles = ps_->particles_;
	XMVECTOR d = XMVectorSet (0.0f, 0.0f, 0.0f, 0.0f);
	
	for (auto i = particles.begin ();
			  i < particles.end ();
			  i++)
		i->a = -1.0f;

	float step = rayData_.range / 3.0 * fabs (rayData_.rayOnly);

	//float realRange = rayData_.range * acos (pow (fabs (rayData_.rayOnly), 1/rayData_.cosPow));

	while (true)
	{
		XMVECTOR d = XMVector3Length (currentPos);
		XMStoreFloat (&distance, d);
		if (distance <= sqrt (3.0f)) break;

		currentPos += currentDir * step;
		iterations++;

		if (iterations > 1000)
		{
			_MessageBox ("Raytracer: The ray didn't intersect the object");
			return;
		}
	}

	printf ("Ray in cube\n");
	iterations = 0;

	XMVECTOR currentPosBak = currentPos;
	XMVECTOR currentDirBak = currentDir;
	float currentNbak = currentN;
	while (true)
	{
		d = XMVector3Length (currentPosBak);
		XMStoreFloat (&distance, d);
		if (distance > sqrt (3.0f)) break;
		iterations++;
		refract_.Process (currentPosBak, currentDirBak, currentNbak);

		currentPosBak += currentDirBak * step;
		if (iterations > 1000)
			break;
	}

	UINT Niterations = iterations;
	iterations = 0;

	size_t size = particles.size ();
	UINT threadsN = std::thread::hardware_concurrency ();
	uint64_t sizeSingle = size / threadsN;
	uint64_t shift = 0;
	
	ThreadData_t data = { &currentPos, rayData_.range };
	
	while (true)
	{
		PrintfProgressBar (iterations, Niterations);
		d = XMVector3Length (currentPos);
		XMStoreFloat (&distance, d);
		if (distance >= sqrt (3.0f)) break;
		data.done = 0;
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

		while (data.done != 4);

		refract_.Process (currentPos, currentDir, currentN);

		currentPos += currentDir * step;
		iterations++;

		shift = 0;
		if (iterations > 1000)
			break;
	}
	_putch ('\n');
	END_EXCEPTION_HANDLING (TRACE_RAY)
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