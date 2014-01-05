#include "stdafx.h"
#include "component.h"
#include "thrustsystem.h"
#include "motionhistory.h"

#include "debug.h"

void ThrustSystem::update()
{
	for (auto id : db->getEntitiesWithComponent<ThrustComponent>())
	{
		ThrustComponent* thrust = nullptr;
		TemporalState2DComponent* state = nullptr;
		db->getComponentsOfTypeForEntity<ThrustComponent, TemporalState2DComponent>(id, &thrust, &state);

		if (!thrust || !state)
			continue;

		motionhistory::add(thrust->current.x, thrust->current.y);

		float thrust_multiplier = 1.0;
		if ((state->current.vel.x > 0.0 && thrust->current.x < 0.0) || (state->current.vel.x < 0.0 && thrust->current.x > 0.0))
			thrust_multiplier = thrust->reverse_thrust_factor;
		thrust->current.x *= thrust_multiplier;
	}
}