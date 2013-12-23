#include "stdafx.h"
#include "physicalcomponent.h"
#include "temporalstatecomponent.h"
#include "thrustcomponent.h"

Vector2D PhysicalComponent::getShipDecelerationFactor(const TemporalState2DComponent& state, const ThrustComponent& thrust)
{
	double y_factor = 4.0;
	if (std::abs(thrust.current.y) > 0)
		y_factor = 0.0;

	double x_factor;
	if (std::abs(state.current.vel.x) > 800 || std::abs(thrust.current.x) > 0)
		x_factor = 0.63;
	else if (std::abs(state.current.vel.x) > 200) // slow more when already slow and no thrust
		x_factor = 1.1;
	else
		x_factor = 2.3;

	return{ x_factor, y_factor };
}

Vector2D PhysicalComponent::getNoDecelerationFactor(const TemporalState2DComponent& state, const ThrustComponent& thrust)
{
	return{ 0.0, 0.0 };
}