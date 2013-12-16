#include "stdafx.h"
#include "physics.h"
#include "component.h"
#include "playfield.h"

void PhysicsSystem::update(double dt)
{
	auto playfield = db->getPlayField();
	auto boundaries = playfield->boundaries;
	for (auto& c : db->getComponentsOfType(C::temporalstate))
	{
		auto state = (TemporalState2DComponent*)c;
		state->prev = state->current;
		state->current.acc.x = thrust->current.x / 1.0;
		state->current.acc.x = (thrust->current.x / (entity->attrib.weight / 3600)) * dt;
		state->current.vel.x += state->current.acc.x * dt;
		state->current.vel.y += thrust->current.y;	// TODO: acceleration

		// decceleration
		Vector2D deceleration_factor = entity->getDecelerationFactor();
		state->current.vel.x -= state->current.vel.x * (deceleration_factor.x * dt);
		state->current.vel.y -= state->current.vel.y * (deceleration_factor.y * dt);

		if (state->current.vel.y > entity->attrib.max_lift) state->current.vel.y = entity->attrib.max_lift;
		if (state->current.vel.y < -entity->attrib.max_lift) state->current.vel.y = -entity->attrib.max_lift;

		state->current.pos.x += (state->current.vel.x * dt) + (state->current.acc.x * 0.5 * dt * dt);
		state->current.pos.y += state->current.vel.y * dt; // TODO: acceleration

		double altitude = playfield->getAltitude(state->current.pos.y);
		if (altitude == 0.0)
		{
			state->current.pos.y = world->ship_limits.y + world->ship_limits.h - entity->box.h;
			state->current.vel.y = 0.0;
		}

		if (state->current.pos.y < world->ship_limits.y)
		{
			state->current.pos.y = world->ship_limits.y;
			state->current.vel.y = 0.0;
		}

		if (state->current.pos.x > world->w)
		{
			state->current.pos.x -= world->w;
			state->current.loop_count++;
		}
		else if (state->current.pos.x < 0.0)
		{
			state->current.pos.x += world->w;
			state->current.loop_count--;
		}

		entity->box.x = state->current.pos.x;
		entity->box.y = state->current.pos.y;

		//entity->updateCollisionBoxes();

		entity->tick(dt);
	}
}

void PhysicsSystem::interpolate(double alpha)
{
	for (auto& c : db->getComponentsOfType(C::temporalstate))
	{
		auto state = (TemporalState2DComponent*)c;
		//if (state->current.loop_count != state->prev.loop_count)
		//{
		//	double wrap_factor = ((entity->state.current.loop_count - entity->state.prev.loop_count)*world->w);
		//	double currentX = entity->state.current.pos.x*alpha;
		//	double prevX = (entity->state.prev.pos.x - wrap_factor)*(1.0 - alpha);
		//	entity->state.interpolated.x = currentX + prevX;
		//}
		//else
		//{
			state->interpolated.x = state->current.pos.x*alpha + state->prev.pos.x*(1.0 - alpha);
		//}
		state->interpolated.y = state->current.pos.y*alpha + state->prev.pos.y*(1.0 - alpha);
	}
}