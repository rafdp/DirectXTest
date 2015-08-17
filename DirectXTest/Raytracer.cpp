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
					  XMFLOAT3 rayStartPos,
					  XMFLOAT3 rayStartDir)
try :
	ps_              (ps),
	rayData_         ({pointColor, rayColor, {cosPow, pointsScale, 0.0f, range}}),
	rayStartPos_     (rayStartPos),
	rayStartDir_     (rayStartDir),
	nConstantBuffer_ ()
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

void Raytracer::TraceRay (float step)
{
	BEGIN_EXCEPTION_HANDLING 
	UINT iterations = 0;
	XMVECTOR currentPos = XMLoadFloat3 (&rayStartPos_);
	XMVECTOR currentDir = XMLoadFloat3 (&rayStartDir_);
	XMFLOAT3 conversion = {};
	float distance = 0.0f;
	std::vector<Vertex_t>& particles = ps_->particles_;
	
	for (auto i = particles.begin ();
			  i < particles.end ();
			  i++)
		i->a = 0.0f;

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

	while (true)
	{
		XMVECTOR d = XMVector3Length (currentPos);
		XMStoreFloat (&distance, d);
		if (distance > sqrt (3.0f)) break;

		for (auto i = particles.begin ();
		i < particles.end ();
			i++)
		{
			XMVECTOR d = XMVector3Length (XMVectorSet (i->r, i->g, i->b, 0.0f) - currentPos);
			XMStoreFloat (&distance, d);
			if (distance < rayData_.range &&
				(distance < i->a ||
				 i->a < 0.1f))
			{
				i->a = distance + 0.2f;
				//_MessageBox ("%f", distance);
			}
		}
		currentPos += currentDir * step;
		iterations++;
		if (iterations > 100)
			break;
	}
	END_EXCEPTION_HANDLING (TRACE_RAY)
}

void Raytracer::SetDrawRayOnly ()
{
	rayData_.rayOnly = 1.0f;
}
void Raytracer::SetDrawAll ()
{
	rayData_.rayOnly = 0.0f;
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
	ps_->PrepareToDraw ();
	END_EXCEPTION_HANDLING (PREPARE_TO_DRAW0)
}
void Raytracer::PrepareToDraw1 ()
{
	BEGIN_EXCEPTION_HANDLING
	SetDrawAll ();
	Update ();
	SendToVS ();
	ps_->PrepareToDraw ();
	END_EXCEPTION_HANDLING (PREPARE_TO_DRAW1)
}

float& Raytracer::GetCosPow ()
{
	return rayData_.cosPow;
}