#pragma once
#include "component_base.h"

class TemporalState2DComponent;
class ThrustComponent;
class PhysicalComponent;

class PoweredBodyComponent : public Component
{
public:
	PoweredBodyComponent(TemporalState2DComponent* state, ThrustComponent* thrust, PhysicalComponent* physical, HorizontalOrientComponent* horient, CollisionComponent* collision) : Component(C::cpoweredbody),
		state{ state },
		thrust{ thrust },
		phys{ physical },
		horient{ horient },
		collision{ collision }
	{}
	TemporalState2DComponent* state;
	ThrustComponent* thrust;
	PhysicalComponent* phys;
	HorizontalOrientComponent* horient;
	CollisionComponent* collision;
};