#pragma once
#include "component_base.h"
#include "phys.h"

class TemporalState2DComponent : public Component
{
public:
	TemporalState2DComponent() : Component() {}
	State2D current;
	State2D prev;
	Point2D interpolated;
};