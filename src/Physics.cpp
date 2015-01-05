#include "stdafx.h"
#include "physics.h"
#include "component.h"
#include "playfield.h"
#include "mathutil.h"

#include <sstream>

namespace
{
	bool testCollide(CollisionComponent& c1, CollisionComponent& c2, PlayField& playfield)
	{
		// Broad sector check (may not really be necessary - outer box checking may be good enough)
		if (!(c2.sector.x == c1.sector.x || (mathutil::abswrap(c2.sector.x - 1, c2.sector_max.x)) == c1.sector.x || mathutil::abswrap(c2.sector.x + 1, c2.sector_max.x) == c1.sector.x))
			return false;
		if (!(c2.sector.y == c1.sector.y || (mathutil::abswrap(c2.sector.y - 1, c2.sector_max.y)) == c1.sector.y || mathutil::abswrap(c2.sector.y + 1, c2.sector_max.y) == c1.sector.y))
			return false;

		// Outer bounds box check
		float outer_rel_offset_x = playfield.getRelativePosX(c2.outer_box.x, c1.outer_box.x);
		Rect c1_outer_box = c1.outer_box;
		c1_outer_box.x = c2.outer_box.x + outer_rel_offset_x;

		if (!mathutil::areRectanglesIntersecting(c1_outer_box, c2.outer_box))
		{
			return false;
		}

		// Inner box checks
		for (auto c1_inner_box : c1.inner_boxes) // note copy, not reference, as we will be modifying to get relative pos
		{
			for (auto& c2_inner_box : c2.inner_boxes) // note reference, not copy
			{
				float inner_rel_offset_x = playfield.getRelativePosX(c2_inner_box.x, c1_inner_box.x);
				c1_inner_box.x = c2_inner_box.x + inner_rel_offset_x;
				if (mathutil::areRectanglesIntersecting(c1_inner_box, c2_inner_box))
				{
					return true;
				}
			}	
		}

		return false;
	}
}

void PhysicsSystem::update(float dt)
{
	for (auto eid : db->getEntitiesWithComponent<FireBulletsComponent>())
	{
		FireBulletsComponent* fire;
		TemporalState2DComponent* state;
		PhysicalComponent* physical;
		HorizontalOrientComponent* horient;
		db->getComponentsOfTypeForEntity<FireBulletsComponent, TemporalState2DComponent, PhysicalComponent, HorizontalOrientComponent>(eid, &fire, &state, &physical, &horient);
		if (fire->isFireRequired())
		{
			unsigned int bullet_id = fire->loadNextBullet();
			auto bullet_state = db->getComponentOfTypeForEntity<TemporalState2DComponent>(bullet_id);
			auto bullet_lifetime = db->getComponentOfTypeForEntity<LifetimeComponent>(bullet_id);
			auto bullet_thrust = db->getComponentOfTypeForEntity<ThrustComponent>(bullet_id);

			bullet_lifetime->reset();
			auto direction_multiplier = (horient->direction == HOrient::right ? 1 : -1);
			bullet_state->current.pos.x = (horient->direction == HOrient::right ? state->current.pos.x + physical->box.w : state->current.pos.x);
			bullet_state->current.pos.y = state->current.pos.y + 24.0;
			bullet_state->current.vel = { state->current.vel.x + (direction_multiplier * 1000.0f), 0.0f };
			bullet_state->prev = bullet_state->current;
			bullet_thrust->current.x = direction_multiplier * bullet_thrust->max.x;
			fire->reset();
		}
	}

	for (auto id : db->getEntitiesWithComponent<TemporalState2DComponent>())
	{
		ThrustComponent* thrust = nullptr;
		TemporalState2DComponent* state = nullptr;
		PhysicalComponent* physical = nullptr;
		HorizontalOrientComponent* horient = nullptr;
		CollisionComponent* collision = nullptr;
		LifetimeComponent* lifetime = nullptr;
		
		db->getComponentsOfTypeForEntity<ThrustComponent, TemporalState2DComponent, PhysicalComponent, HorizontalOrientComponent, CollisionComponent, LifetimeComponent>(
			id, &thrust, &state, &physical, &horient, &collision, &lifetime
			);

		if (lifetime)
		{
			if (!lifetime->active)
				continue;
			lifetime->tick(dt);
		}

		auto playfield = db->getPlayField();
		auto boundaries = playfield->boundaries;

		state->prev = state->current;

		// acceleration
		if (thrust)
		{
			state->current.acc.x = (thrust->current.x / physical->weight);
			state->current.acc.y = (thrust->current.y / physical->weight);
		}
		state->current.vel.x += state->current.acc.x * dt;
		state->current.vel.y += state->current.acc.y * dt;

		// deceleration (from reverse thrust and/or natural atmospheric deceleration)
		Vector2D deceleration_factor = physical->getDecelerationFactor(*state, thrust);
		state->current.vel.x -= state->current.vel.x * (deceleration_factor.x * dt);
		state->current.vel.y -= state->current.vel.y * (deceleration_factor.y * dt);

		if (!state->ignore_acc.x)
			state->current.pos.x += (state->current.vel.x * dt) + (state->current.acc.x * 0.5 * dt * dt);
		else
			state->current.pos.x += state->current.vel.x * dt;
		state->current.pos.x = mathutil::abswrap(state->current.pos.x, playfield->w);

		if (!state->ignore_acc.y)
			state->current.pos.y += state->current.vel.y * dt + (state->current.acc.y * 0.5 * dt * dt);
		else
			state->current.pos.y += state->current.vel.y * dt;

		float altitude = playfield->getAltitude(state->current.pos.y, physical->box.h);
		if (altitude == 0.0)
		{
			state->current.pos.y = boundaries.y + boundaries.h - physical->box.h;
			state->current.vel.y = 0.0;
		}
		else if (state->current.pos.y < boundaries.y)
		{
			state->current.pos.y = boundaries.y;
			state->current.vel.y = 0.0;
		}

		physical->box.x = state->current.pos.x;
		physical->box.y = state->current.pos.y;

		if (collision)
		{
			collision->updateCollisionBoxes(state, horient, playfield->w, playfield->boundaries.h);
		}

		//entity->tick(dt);

		if (horient) // i.e. ship
		{
			//debug::set("x", state->current.pos.x);
			//debug::set("vel.x", state->current.vel.x);
			//debug::set("acc.x", state->current.acc.x);
			//debug::set("thrust.x", thrust->current.x);

			//debug::set("y", state->current.pos.y);
			//debug::set("current.vel.y", state->current.vel.y);
			//debug::set("current.acc.y", state->current.acc.y);
			//debug::set("thrust.y", thrust->current.y);

			//debug::set("deceleration", std::to_string(deceleration_factor.x) + " / " + std::to_string(deceleration_factor.y));
		}
	}
}

void PhysicsSystem::interpolate(float alpha)
{
	auto playfield = db->getPlayField();
	for (auto& state : db->getComponentsOfType<TemporalState2DComponent>())
	{
		float delta_x = playfield->getRelativePosX(state->current.pos.x, state->prev.pos.x);
		state->interpolated.x = state->current.pos.x + (delta_x)*alpha;
		state->interpolated.y = state->current.pos.y*alpha + state->prev.pos.y*(1.0 - alpha);
	}
}

void PhysicsSystem::collide()
{
	auto playfield = db->getPlayField();
	for (auto collide : db->getComponentsOfType<CollisionComponent>())
	{
		collide->is_collided = false;
	}

	auto ship_id = db->getEntitiesOfType(E::eship)[0];
	auto ship_collide = db->getComponentOfTypeForEntity<CollisionComponent>(ship_id);
	//debug::set("ship_sector.x", ship_collide->sector.x);
	//debug::set("ship_sector.y", ship_collide->sector.y);

	std::vector<CollisionComponent*> bullet_collides;
	for (auto bullet_id : db->getEntitiesOfType(E::ebullet))
	{
		CollisionComponent* bullet_collide = nullptr;
		LifetimeComponent* bullet_lifetime = nullptr;
		db->getComponentsOfTypeForEntity<CollisionComponent, LifetimeComponent>(bullet_id, &bullet_collide, &bullet_lifetime);
		if (!bullet_lifetime->active)
			continue;
		bullet_collides.push_back(bullet_collide);
	}

	for (auto alien_collide : db->getComponentsOfTypeForEntityType<CollisionComponent>(E::ealien))
	{
		if (ship_collide && testCollide(*ship_collide, *alien_collide, *playfield))
		{
			ship_collide->is_collided = true;
			alien_collide->is_collided = true;
		}

		for (auto bullet_collide : bullet_collides)
		{
			if (testCollide(*bullet_collide, *alien_collide, *playfield))
			{
				bullet_collide->is_collided = true;
				alien_collide->is_collided = true;
			}
		}
	}

	for (auto station_id : db->getEntitiesOfType(E::estation))
	{
		auto station_collide = db->getComponentOfTypeForEntity<CollisionComponent>(station_id);
		auto station_st = db->getComponentOfTypeForEntity<StationComponent>(station_id);

		if (ship_collide && testCollide(*ship_collide, *station_collide, *playfield))
		{
			station_st->is_docked = true;
			ship_collide->is_collided = true;
		}
		else
		{
			station_st->is_docked = false;
		}
	}
}