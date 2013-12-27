#pragma once
#include "component_base.h"
#include "phys.h"

class ThrustComponent : public Component
{
public:
	ThrustComponent(Vector2D max = { 0, 0 }, double reverse_thrust_factor = 1.0) : Component(),
		max{ max },
		reverse_thrust_factor{ reverse_thrust_factor }
	{}
	Vector2D current;
	Vector2D prev;
	Vector2D max;
	double reverse_thrust_factor;
	void resetCurrent() { current = { 0, 0 }; }
};