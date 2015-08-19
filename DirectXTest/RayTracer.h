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
class Raytracer;

struct ThreadData_t
{
	XMVECTOR* currentPos;
	float range;
	Vertex_t* data;
	uint64_t size;
	volatile bool read;
	volatile std::atomic<char> done;

	ThreadData_t (const ThreadData_t& that);
	ThreadData_t (XMVECTOR* currentPos_, float range_);
};

DWORD WINAPI ThreadedRaytracing (void* ptr);


class Raytracer : NZA_t
{
	ParticleSystem* ps_;
	RaytracerCBData_t rayData_;
	XMFLOAT3 rayStartPos_;
	XMFLOAT3 rayStartDir_;
	ConstantBufferIndex_t nConstantBuffer_;
	Rayrefractor refract_;
	volatile bool stop_;
	volatile bool ready_;
	float a;

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

	static DWORD WINAPI ParallelProcessing (void* ptr);

	void SetCosPow     (float setting);
	void SetRange      (float setting);
	void SetScale      (float setting);
	void SetCosClip    (float setting);
	void SetRayColor   (XMFLOAT4 color);
	void SetRayColorR  (float setting);
	void SetRayColorG  (float setting);
	void SetRayColorB  (float setting);
	void SetRayColorA  (float setting);
	void SetTopTemp    (float setting);
	void SetBottomTemp (float setting);
	void SetTransition (float setting);

	void Join ();
	void Stop ();
	void Clear ();
};

class ScriptCompiler : NZA_t
{
	std::string filename_;
	Raytracer* rt_;
public:

	void ok ();
	ScriptCompiler (std::string filename,
					Raytracer* rt);
	~ScriptCompiler ();
	friend DWORD WINAPI ScriptThread (void* ptr);

	void Run ();
};