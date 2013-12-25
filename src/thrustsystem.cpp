#include "stdafx.h"
#include "component.h"
#include "thrustsystem.h"

#include "debug.h"

void ThrustSystem::update()
{
	for (auto eid : db->getEntitiesWithComponent(C::cthrust))
	{
		auto thrust = (ThrustComponent*)db->getComponentOfTypeForEntity(eid, C::cthrust);
		auto state = (TemporalState2DComponent*)db->getComponentOfTypeForEntity(eid, C::ctemporalstate);

		if (!thrust || !state)
			continue;

		double thrust_multiplier = 1.0;
		if ((state->current.vel.x > 0.0 && thrust->current.x < 0.0) || (state->current.vel.x < 0.0 && thrust->current.x > 0.0))
			thrust_multiplier = thrust->reverse_thrust_factor;
		thrust->current.x *= thrust_multiplier;
	}
}