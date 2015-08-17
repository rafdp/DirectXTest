#pragma once

#include "includes.h"
#pragma pack (push, 16)
struct RaytracerCBData_t
{
	XMFLOAT4 pointColor;
	XMFLOAT4 rayColor;
	struct {
		float cosPow;
		float pointsScale;
		float rayOnly;
		float range;
	};
};
#pragma pack (pop)

struct ThreadData_t
{
	XMVECTOR* currentPos;
	float range;
	Vertex_t* data;
	uint64_t size;
	volatile bool read;
	volatile uint8_t done;
};

DWORD WINAPI ThreadedRaytracing (void* ptr);


class Raytracer : NZA_t
{
	ParticleSystem* ps_;
	RaytracerCBData_t rayData_;
	XMFLOAT3 rayStartPos_;
	XMFLOAT3 rayStartDir_;
	UINT nConstantBuffer_;

public:

	void ok ();

	Raytracer (ParticleSystem* ps,
			   UINT cbSlot,
			   XMFLOAT4 pointColor,
			   XMFLOAT4 rayColor,
			   float cosPow,
			   float pointsScale,
			   float range,
			   float cosClipping,
			   XMFLOAT3 rayStartPos,
			   XMFLOAT3 rayStartDir);

	~Raytracer ();

	void TraceRay ();

	void SetDrawRayOnly ();
	void SetDrawAll ();
	void Update ();
	void SendToGS ();
	void SendToVS ();
	void PrepareToDraw0 ();
	void PrepareToDraw1 ();
	float& GetCosPow ();
};

