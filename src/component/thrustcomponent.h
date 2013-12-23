#pragma once
#include "component_base.h"
#include "phys.h"

class ThrustComponent : public Component
{
public:
	ThrustComponent() : Component(C::thrust) {}
	Vector2D current;
	Vector2D prev;
	Vector2D max;
	double reverse_thrust_factor{ 1.0 };
	void resetCurrent() { current = { 0, 0 }; }
};