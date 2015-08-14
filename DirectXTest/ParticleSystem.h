#pragma once
#include "includes.h"

struct POINT3D
{
	float x, y, z;
};


class ParticleSystem : NZA_t
{
	std::vector<Vertex_t> particlesRender;
	std::vector<POINT3D>  particlesPos;

public:

	void ok ();

	ParticleSystem (float xMin, float xMax,
					float yMin, float yMax,
					float zMin, float zMax,
					UINT quantity, float scale,
					float r, float g, float b, float a,
					float colorScatter = 0.0f);
	~ParticleSystem ();

	void DumpToObject (Direct3DObject* drawing);

	void ApplyRay (float r, float g, float b,
				   float x, float y, float z,
				   float vx, float vy, float vz,
				   float d);

};