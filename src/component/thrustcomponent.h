#pragma once
#include "component_base.h"
#include "phys.h"

class ThrustComponent : public Component
{
public:
	ThrustComponent(Vector2D max = { 0, 0 }, float reverse_thrust_factor = 1.0, bool is_motion_tracked = false) : Component(),
		max{ max },
		reverse_thrust_factor{ reverse_thrust_factor },
		is_motion_tracked{ is_motion_tracked }
	{}
	Vector2D current;
	Vector2D prev;
	Vector2D max;
	bool is_motion_tracked;
	float reverse_thrust_factor;
	void resetCurrent() { current = { 0, 0 }; }
};