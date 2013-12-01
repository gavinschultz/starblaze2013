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

void physics::integrate(double delta_time, double dt)
{
	for (auto& entity : game->entity_register.getAll())
	{
		if (!entity->is_active)
			continue;

		entity->prev_state = entity->current_state;
		entity->current_state.acc.x = entity->current_state.thrust.x / entity->weight;
		entity->current_state.vel.x += entity->current_state.acc.x * dt;
		entity->current_state.vel.y += entity->current_state.thrust.y;	// TODO: acceleration

		// decceleration
		entity->current_state.vel.x -= entity->current_state.vel.x * (entity->getDecelerationFactorX() * dt);
		entity->current_state.vel.y -= entity->current_state.vel.y * (entity->getDecelerationFactorY() * dt);

		if (entity->current_state.vel.y > entity->max_lift) entity->current_state.vel.y = entity->max_lift;
		if (entity->current_state.vel.y < -entity->max_lift) entity->current_state.vel.y = -entity->max_lift;

		entity->current_state.pos.x += (entity->current_state.vel.x * dt) + (entity->current_state.acc.x * 0.5 * dt * dt);
		entity->current_state.pos.y += entity->current_state.vel.y * dt; // TODO: acceleration

		entity->altitude = std::max(0.0, world->ship_limits.y + world->ship_limits.h - entity->current_state.pos.y - entity->bounding_box.h);
		if (entity->altitude == 0.0)
		{
			entity->current_state.pos.y = world->ship_limits.y + world->ship_limits.h - entity->bounding_box.h;
			entity->current_state.vel.y = 0.0;
		}

		if (entity->current_state.pos.y < world->ship_limits.y)
		{
			entity->current_state.pos.y = world->ship_limits.y;
			entity->current_state.vel.y = 0.0;
		}

		if (entity->current_state.pos.x > world->w)
		{
			entity->current_state.pos.x -= world->w;
			entity->current_state.loop_count++;
		}
		else if (entity->current_state.pos.x < 0.0)
		{
			entity->current_state.pos.x += world->w;
			entity->current_state.loop_count--;
		}

		entity->bounding_box.x = entity->current_state.pos.x;
		entity->bounding_box.y = entity->current_state.pos.y;

		entity->updateCollisionBoxes();

		if (entity->weight == 0.5)
		{
			console_debug({ "bullet.x: ", std::to_string(entity->current_state.pos.x) });
		}

		entity->tick(dt);
	}
}

void physics::interpolate(double alpha)
{
	for (auto& entity : game->entity_register.getAll())
	{
		if (entity->current_state.loop_count != entity->prev_state.loop_count)
		{
			double wrap_factor = ((entity->current_state.loop_count - entity->prev_state.loop_count)*world->w);
			double currentX = entity->current_state.pos.x*alpha;
			double prevX = (entity->prev_state.pos.x - wrap_factor)*(1.0 - alpha);
			entity->alpha_pos.x = currentX + prevX;
		}
		else
		{
			entity->alpha_pos.x = entity->current_state.pos.x*alpha + entity->prev_state.pos.x*(1.0 - alpha);
		}
		entity->alpha_pos.y = entity->current_state.pos.y*alpha + entity->prev_state.pos.y*(1.0 - alpha);
		entity->updateCollisionBoxes();
	}
}

void physics::collisionDetection()
{
	auto ship = game->entity_register.getShip();
	ship->is_collided = false;
	for (auto& alien : game->entity_register.getAliens())
	{
		alien->is_collided = false;
		if (util::areRectanglesIntersecting(ship->bounding_box, alien->bounding_box))
		{
			alien->is_collided = true;
			ship->is_collided = true;
		}
	}
}