#pragma once
#include "component_base.h"
#include "phys.h"

class TemporalState2DComponent;
class ThrustComponent;

class PhysicalComponent : public Component
{
public:
	PhysicalComponent() : Component(C::cphysical) {}
	Rect box;
	double weight{ 0.0 };
	Vector2D(*getDecelerationFactor)(const TemporalState2DComponent& state, const ThrustComponent* thrust) { PhysicalComponent::getNoDecelerationFactor };

	static Vector2D getShipDecelerationFactor(const TemporalState2DComponent& state, const ThrustComponent* thrust);
	static Vector2D getNoDecelerationFactor(const TemporalState2DComponent& state, const ThrustComponent* thrust);
};