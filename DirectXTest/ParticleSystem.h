#pragma once
#include "includes.h"

struct Ray
{
	XMFLOAT4 color;
	XMFLOAT4 point;
	XMFLOAT4 vector;
	float range;
	float pow;
	float scale;
	float rayOnly;
	UINT nConstantBuffer;
	Direct3DProcessor* d3dProc;

	Ray (UINT slot, 
		 Direct3DProcessor* d3dProc_,
		 XMFLOAT4 color_,
		 XMFLOAT4 point_,
		 XMFLOAT4 vector_,
		 float min_,
		 float pow_,
		 float scale_);

	void Update ();
	void SendToVS ();
	void SendToGS ();
	void SetRayOnly ();
	void SetAll ();
};


class ParticleSystem : NZA_t
{
	std::vector<Vertex_t> particles_;
	Direct3DObject* object_;
	Direct3DProcessor* proc_;
	Ray* ray_;
	UINT samplerN_;
	UINT textureN_;

public:

	void ok ();

	ParticleSystem (Direct3DObject* drawing,
					Direct3DProcessor* proc,
					Ray* ray,
					float xMin, float xMax,
					float yMin, float yMax,
					float zMin, float zMax,
					UINT quantity,
					float r, float g, float b, float a,
					float colorScatter = 0.0f);

	~ParticleSystem ();


	void PrepareToDraw0 ();
	void PrepareToDraw1 ();
	/*
	void ApplyRay (float r, float g, float b,
				   float x, float y, float z,
				   float vx, float vy, float vz,
				   float d);
	*/

};