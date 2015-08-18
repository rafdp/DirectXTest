#pragma once

#include "includes.h"

class Rayrefractor : NZA_t
{
	float temperatureTop_;
	float temperatureBottom_;
	float transitionLength_;

	float N (float t);

public:

	void ok ();

	Rayrefractor (float temperatureTop, 
				  float temperatureBottom,
				  float transitionLength);

	~Rayrefractor ();

	void Process (const XMVECTOR& pos, XMVECTOR& dir, float& storedN);

	void SetTopTemperature (float temp);

	void SetBottomTemperature (float temp);

	void SetTransitionLength (float length);
};