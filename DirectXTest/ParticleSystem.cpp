#include "Builder.h"


Ray::Ray (UINT slot,
		  Direct3DProcessor* d3dProc_,
		  XMFLOAT4 color_,
		  XMFLOAT4 point_,
		  XMFLOAT4 vector_,
		  float range_,
		  float pow_,
		  float scale_) :
	color  (color_),
	point  (point_),
	vector (vector_),
	range  (range_),
	pow    (pow_),
	scale  (scale_),
	nConstantBuffer (),
	d3dProc (d3dProc_)
{
	nConstantBuffer = d3dProc->RegisterConstantBuffer (&color, 64, slot);
	Update ();
}

void Ray::Update ()
{
	d3dProc->UpdateConstantBuffer (nConstantBuffer);
}
void Ray::SendToVS ()
{
	d3dProc->SendCBToVS (nConstantBuffer);
}
void Ray::SendToGS ()
{
	d3dProc->SendCBToGS (nConstantBuffer);
}

void ParticleSystem::ok ()
{
	DEFAULT_OK_BLOCK
}

ParticleSystem::ParticleSystem (float xMin, float xMax,
								float yMin, float yMax,
								float zMin, float zMax,
								UINT quantity,
								float r, float g, float b, float a,
								float colorScatter)
try :
	particles ()
{
	Vertex_t vertex = {};
	XMFLOAT3 color = {};
	particles.reserve (quantity + 1);
	for (UINT i = 0; i < quantity; i++)
	{
		vertex.x = (rand () * 1.0f / RAND_MAX) * (xMax - xMin) + xMin;
		vertex.y = (rand () * 1.0f / RAND_MAX) * (yMax - yMin) + yMin;
		vertex.z = (rand () * 1.0f / RAND_MAX) * (zMax - zMin) + zMin;

#define RAND_COLOR(a_, b_) \
		color.a_ = b_ + 2 * colorScatter * (rand () * 1.0f / RAND_MAX - 0.5f); \
		if (color.a_ > 1.0f) color.a_ = 1.0f; \
		if (color.a_ < 0.0f) color.a_ = 0.0f

		RAND_COLOR (x, r);
		RAND_COLOR (y, g);
		RAND_COLOR (z, b);
#undef RAND_COLOR

		vertex.SetColor (color.x, color.y, color.z, a);
		particles.push_back (vertex);
	}
}
_END_EXCEPTION_HANDLING (CTOR)

ParticleSystem::~ParticleSystem ()
{
	particles.clear ();
}

void ParticleSystem::DumpToObject (Direct3DObject* drawing)
{
	drawing->AddVertexArray (particles.data (), 
							 particles.size ());
}
/*
void ParticleSystem::ApplyRay (float r, float g, float b,
							   float x, float y, float z,
							   float vx, float vy, float vz,
							   float d)
{
	XMVECTOR color  = XMVectorSet (r, g, b, 0.0f);
	XMVECTOR point  = XMVectorSet (x, y, z, 1.0f);
	XMVECTOR vector = XMVectorSet (vx, vy, vz, 0.0f);
	XMVECTOR currentPos;
	XMVECTOR currentColor;
	XMFLOAT4 transform = {};
	float currentD = 0.0f;
	for (UINT i = 0;
			  i < particlesPos.size ();
		      i++)
	{
		currentPos = XMVectorSet (particlesPos[i].x,
								  particlesPos[i].y,
								  particlesPos[i].z,
								  0.0f);
		XMStoreFloat4 (&transform, (XMVector3Length (XMVector3Cross (point - currentPos,
																	 vector))));
		currentD = transform.x;
		XMStoreFloat4 (&transform, XMVector3Length (vector));
		currentD /= transform.x;
		_MessageBox ("%f %f %f\n%f %f %f\n%f %f %f\n%f",
					 x, y, z, vx, vy, vz,
					 particlesPos[i].x,
					 particlesPos[i].y,
					 particlesPos[i].z,
					 currentD);
		if (currentD < d)
		{
			
			currentColor = XMVectorSet (particlesRender[3 * i + 0].r,
										particlesRender[3 * i + 0].g,
										particlesRender[3 * i + 0].b,
										particlesRender[3 * i + 0].a);

			currentColor = XMVectorMultiply (color, currentColor);
				
			XMVectorSaturate (currentColor);
			XMStoreFloat4 (&transform, currentColor);
			particlesRender[3 * i + 0].SetColor (transform.x, transform.y, transform.z, particlesRender[3 * i + 0].a);
			particlesRender[3 * i + 1].SetColor (transform.x, transform.y, transform.z, particlesRender[3 * i + 0].a);
			particlesRender[3 * i + 2].SetColor (transform.x, transform.y, transform.z, particlesRender[3 * i + 0].a);
			
			particlesRender[3 * i + 0].SetColor (1.0f, 1.0f, 1.0f, particlesRender[3 * i + 0].a);
			particlesRender[3 * i + 1].SetColor (1.0f, 1.0f, 1.0f, particlesRender[3 * i + 0].a);
			particlesRender[3 * i + 2].SetColor (1.0f, 1.0f, 1.0f, particlesRender[3 * i + 0].a);
		}


	}
}
*/