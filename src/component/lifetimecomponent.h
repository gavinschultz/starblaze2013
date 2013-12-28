#pragma once
#include "component_base.h"

class LifetimeComponent : public Component
{
public:
	LifetimeComponent(double max_age = 1.0, bool active = false) : Component(), max_age(max_age) 
	{
		Component::active = active;
	}
	const double max_age;
	double age{ 0.0 };
	void tick(double dt)
	{
		if (!active)
			return;

		age += dt;
		if (age > max_age)
		{
			active = false;
		}
	}
	void reset()
	{
		age = 0.0;
		active = true;
	}
};