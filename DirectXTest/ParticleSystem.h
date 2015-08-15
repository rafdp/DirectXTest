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
	float pad;
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
};


class ParticleSystem : NZA_t
{
	std::vector<Vertex_t> particles;

public:

	void ok ();

	ParticleSystem (float xMin, float xMax,
					float yMin, float yMax,
					float zMin, float zMax,
					UINT quantity,
					float r, float g, float b, float a,
					float colorScatter = 0.0f);
	~ParticleSystem ();

	void DumpToObject (Direct3DObject* drawing);
	/*
	void ApplyRay (float r, float g, float b,
				   float x, float y, float z,
				   float vx, float vy, float vz,
				   float d);
	*/

};