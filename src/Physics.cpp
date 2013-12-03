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

// TODO: this is still pretty inefficient, as we're comparing every object to every other, even if they're miles apart.
void physics::collisionDetection()
{
	auto ship = game->entity_register.getShip();
	ship->is_collided = false;
	for (auto& alien : game->entity_register.getAliens())
	{
		if (!alien->is_active)
			continue;
		
		alien->is_collided = false;
		if (util::areRectanglesIntersecting(ship->bounding_box, alien->bounding_box))
		{
			alien->is_collided = true;
			ship->is_collided = true;
		}

		for (auto& bullet : game->entity_register.getBullets())
		{
			if (!bullet->is_active)
				continue;

			// TODO: would probably be better in this case to intersect the alien->bounding_box to the line (bullet.prev_state.pos, bullet.current_state.pos)
			if (util::areRectanglesIntersecting(bullet->bounding_box, alien->bounding_box))
			{
				alien->is_collided = true;
				bullet->is_collided = true;
				bullet->is_active = false;

			}
		}

		alien->is_active = alien->is_active && !alien->is_collided;
		if (alien->isInactive())
		{
			game->entity_register.registerEntity(new Debris{ alien->current_state });
		}
	}

	game->entity_register.removeInactives();

	auto station = game->entity_register.getStation();
	if (station && station->is_active)
	{
		station->is_docked = false;
		if (util::areRectanglesIntersecting(ship->bounding_box, station->bounding_box))
		{
			station->is_docked = true;
		}
	}

	//ship->is_active = !ship->is_collided;
}