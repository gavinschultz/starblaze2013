#include "stdafx.h"
#include "physicalcomponent.h"
#include "temporalstatecomponent.h"
#include "thrustcomponent.h"

Vector2D PhysicalComponent::getShipDecelerationFactor(const TemporalState2DComponent& state, const ThrustComponent* thrust)
{
	float thrust_x = 0.0, thrust_y = 0.0;
	if (thrust)
	{
		thrust_x = thrust->current.x;
		thrust_y = thrust->current.y;
	}

	float y_factor = 4.0;
	if (std::abs(thrust_y) > 0)
		y_factor = 0.0;

	float x_factor;
	if (std::abs(state.current.vel.x) > 800 || std::abs(thrust_x) > 0)
		x_factor = 0.63;
	else if (std::abs(state.current.vel.x) > 200) // slow more when already slow and no thrust
		x_factor = 1.1;
	else
		x_factor = 2.3;

	return{ x_factor, y_factor };
}

Vector2D PhysicalComponent::getNoDecelerationFactor(const TemporalState2DComponent& state, const ThrustComponent* thrust)
{
	return{ 0.0, 0.0 };
}