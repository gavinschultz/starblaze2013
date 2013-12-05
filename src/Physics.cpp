#include "stdafx.h"
#include <algorithm>
#include "Physics.h"
#include "Game.h"
#include "Util.h"
#include "Entity\Entity.h"
#include "Entity\World.h"
#include "Entity\Ship.h"
#include "Entity\Alien.h"
#include "Entity\Bullet.h"
#include "Entity\Station.h"
#include "Entity\Debris.h"

void physics::integrate(double delta_time, double dt)
{
	for (auto& entity : game->entity_register.getAll())
	{
		if (!entity->is_active)
			continue;

		entity->state.prev = entity->state.current;
		entity->state.current.acc.x = entity->state.current.thrust.x / 1.0;
		entity->state.current.acc.x = (entity->state.current.thrust.x / (entity->attrib.weight / 3600)) * dt;
		entity->state.current.vel.x += entity->state.current.acc.x * dt;
		entity->state.current.vel.y += entity->state.current.thrust.y;	// TODO: acceleration

		// decceleration
		Vector2D deceleration_factor = entity->getDecelerationFactor();
		entity->state.current.vel.x -= entity->state.current.vel.x * (deceleration_factor.x * dt);
		entity->state.current.vel.y -= entity->state.current.vel.y * (deceleration_factor.y * dt);

		if (entity->state.current.vel.y > entity->attrib.max_lift) entity->state.current.vel.y = entity->attrib.max_lift;
		if (entity->state.current.vel.y < -entity->attrib.max_lift) entity->state.current.vel.y = -entity->attrib.max_lift;

		entity->state.current.pos.x += (entity->state.current.vel.x * dt) + (entity->state.current.acc.x * 0.5 * dt * dt);
		entity->state.current.pos.y += entity->state.current.vel.y * dt; // TODO: acceleration

		double altitude = world->getAltitude(*entity);
		if (altitude == 0.0)
		{
			entity->state.current.pos.y = world->ship_limits.y + world->ship_limits.h - entity->box.h;
			entity->state.current.vel.y = 0.0;
		}

		if (entity->state.current.pos.y < world->ship_limits.y)
		{
			entity->state.current.pos.y = world->ship_limits.y;
			entity->state.current.vel.y = 0.0;
		}

		if (entity->state.current.pos.x > world->w)
		{
			entity->state.current.pos.x -= world->w;
			entity->state.current.loop_count++;
		}
		else if (entity->state.current.pos.x < 0.0)
		{
			entity->state.current.pos.x += world->w;
			entity->state.current.loop_count--;
		}

		entity->box.x = entity->state.current.pos.x;
		entity->box.y = entity->state.current.pos.y;

		entity->updateCollisionBoxes();

		entity->tick(dt);
	}
}

void physics::interpolate(double alpha)
{
	for (auto& entity : game->entity_register.getAll())
	{
		if (entity->state.current.loop_count != entity->state.prev.loop_count)
		{
			double wrap_factor = ((entity->state.current.loop_count - entity->state.prev.loop_count)*world->w);
			double currentX = entity->state.current.pos.x*alpha;
			double prevX = (entity->state.prev.pos.x - wrap_factor)*(1.0 - alpha);
			entity->state.interpolated.x = currentX + prevX;
		}
		else
		{
			entity->state.interpolated.x = entity->state.current.pos.x*alpha + entity->state.prev.pos.x*(1.0 - alpha);
		}
		entity->state.interpolated.y = entity->state.current.pos.y*alpha + entity->state.prev.pos.y*(1.0 - alpha);
	}
}

// TODO: this is still pretty inefficient, as we're comparing every object to every other, even if they're miles apart.
void physics::collisionDetection()
{
	for (auto& entity : game->entity_register.getAll())
	{
		if (!entity->is_active)
			continue;

		for (auto& collider : entity->getColliders())
		{
			collider->test(entity);
		}
	}

	game->entity_register.removeInactives();
}