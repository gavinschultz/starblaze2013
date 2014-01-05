#pragma once
#include "component_base.h"
#include "phys.h"

class TemporalState2DComponent;
class ThrustComponent;

class PhysicalComponent : public Component
{
public:
	PhysicalComponent(Rect box = { 0.0, 0.0, 0.0, 0.0 }, float weight = 1.0) : Component(), box(box), weight(weight) {}
	Rect box;
	float weight;
	Vector2D(*getDecelerationFactor)(const TemporalState2DComponent& state, const ThrustComponent* thrust) { PhysicalComponent::getNoDecelerationFactor };

	static Vector2D getShipDecelerationFactor(const TemporalState2DComponent& state, const ThrustComponent* thrust);
	static Vector2D getNoDecelerationFactor(const TemporalState2DComponent& state, const ThrustComponent* thrust);
};