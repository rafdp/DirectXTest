#include "Builder.h"

void Rayrefractor::ok ()
{
	DEFAULT_OK_BLOCK
}

float Rayrefractor::N (float t)
{
	static const float A = -1.304f*pow (10, -6);
	static const float B = -3.633f*pow (10, -5);
	static const float C = 1.333f;
	return A*t*t + B*t + C;
}

Rayrefractor::Rayrefractor (float temperatureTop,
							float temperatureBottom,
							float transitionLength)
try :
	temperatureTop_    (temperatureTop),
	temperatureBottom_ (temperatureBottom),
	transitionLength_  (transitionLength)
{}
_END_EXCEPTION_HANDLING (CTOR)

Rayrefractor::~Rayrefractor ()
{
	temperatureTop_ = -1.0f;
	temperatureBottom_ = -1.0f;
	transitionLength_ = -1.0f;
}

void Rayrefractor::SetTopTemperature (float temp)
{
	if (temp >= 0.0f && temp <= 95.0f)
		temperatureTop_ = temp;
}

void Rayrefractor::SetBottomTemperature (float temp)
{
	if (temp >= 0.0f && temp <= 95.0f)
		temperatureBottom_ = temp;
}

void Rayrefractor::SetTransitionLength (float length)
{
	if (length >= 0.0f && length <= 2.0f)
		transitionLength_ = length;
}

void Rayrefractor::Process (const XMVECTOR& pos, 
							XMVECTOR& dir, 
							float& storedN)
{
	float currentN = 0.0f;
	XMFLOAT3 coords = {};
	XMStoreFloat3 (&coords, pos);
	if (coords.y > transitionLength_ / 2.0f)
		currentN = 1.5f/*N (temperatureTop_)*/;
	else
	if (coords.y < -transitionLength_ / 2.0f)
		currentN = 1.0f/*N (temperatureBottom_)*/;
	else
	{
		float Ntop = N (temperatureTop_);
		float Nbottom = N (temperatureBottom_);

		float part = (coords.y + transitionLength_ / 2.0f) / transitionLength_;

		currentN = Ntop * part + Nbottom * (1 - part);
	}
	dir = XMVector3Normalize (dir);
	if (fabs (storedN - currentN) < 0.001f) return;
	XMStoreFloat3 (&coords, dir);

	float y2 = coords.y*coords.y;
	float k = storedN / currentN;
	float a = coords.x*coords.x / coords.z*coords.z;

	coords.x = (coords.x < 0 ? (-1) : (+1)) * sqrt (k*(1.0f-y2) / (1 + 1/a));
	coords.y = (coords.y < 0 ? (-1) : (+1)) * sqrt (1 - k + y2 * k);
	coords.z = (coords.z < 0 ? (-1) : (+1)) * sqrt (k*(1.0f - y2) / (1 + a));

	dir = XMLoadFloat3 (&coords);
	storedN = currentN;
}