#pragma once
#include "component_base.h"
#include "phys.h"

class TemporalState2DComponent : public Component
{
public:
	TemporalState2DComponent(State2D current = State2D{}) : Component(), current(current) {}
	State2D current;
	State2D prev;
	Point2D interpolated;
};