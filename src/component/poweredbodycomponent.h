#pragma once
#include "component_base.h"

class TemporalState2DComponent;
class ThrustComponent;
class PhysicalComponent;

class PoweredBodyComponent : public Component
{
public:
	PoweredBodyComponent() : Component(C::poweredbody) {}
	TemporalState2DComponent* state;
	ThrustComponent* thrust;
	PhysicalComponent* phys;
	HorizontalOrientComponent* horient;
	CollisionComponent* collision;
};