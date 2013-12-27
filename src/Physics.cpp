#include "stdafx.h"
#include "physics.h"
#include "component.h"
#include "playfield.h"
#include "mathutil.h"

void PhysicsSystem::update(double dt)
{
	auto playfield = db->getPlayField();
	auto boundaries = playfield->boundaries;

	for (auto eid : db->getEntitiesWithComponent<FireBulletsComponent>())
	{
		auto fire = db->getComponentOfTypeForEntity<FireBulletsComponent>(eid);
		auto state = db->getComponentOfTypeForEntity<TemporalState2DComponent>(eid);
		auto horient = db->getComponentOfTypeForEntity<HorizontalOrientComponent>(eid);

		if (fire->isFireRequired())
		{
			
		}
	}

	for (auto id : db->getEntitiesWithComponent<TemporalState2DComponent>())
	{
		auto thrust = db->getComponentOfTypeForEntity<ThrustComponent>(id);
		auto state = db->getComponentOfTypeForEntity<TemporalState2DComponent>(id);
		auto physical = db->getComponentOfTypeForEntity<PhysicalComponent>(id);
		auto horient = db->getComponentOfTypeForEntity<HorizontalOrientComponent>(id);
		auto collision = db->getComponentOfTypeForEntity<CollisionComponent>(id);

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

		state->current.pos.x += (state->current.vel.x * dt) + (state->current.acc.x * 0.5 * dt * dt);
		state->current.pos.x = mathutil::abswrap(state->current.pos.x, playfield->w);

		state->current.pos.y += state->current.vel.y * dt + (state->current.acc.y * 0.5 * dt * dt);
		double altitude = playfield->getAltitude(state->current.pos.y, physical->box.h);
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
			collision->updateCollisionBoxes(state, horient);
		}

		//entity->tick(dt);

		//debug::set("x", state->current.pos.x);
		//debug::set("vel.x", state->current.vel.x);
		//debug::set("acc.x", state->current.acc.x);
		//debug::set("thrust.x", thrust->current.x);
		
		//debug::set("y", state->current.pos.y);
		//debug::set("vel.y", state->current.vel.y);
		//debug::set("acc.y", state->current.acc.y);
		//debug::set("thrust.y", thrust->current.y);

		//debug::set("deceleration", std::to_string(deceleration_factor.x) + " / " + std::to_string(deceleration_factor.y));
	}
}

void PhysicsSystem::interpolate(double alpha)
{
	auto playfield = db->getPlayField();
	for (auto& state : db->getComponentsOfType<TemporalState2DComponent>())
	{
		double delta_x = playfield->getRelativePosX(state->current.pos.x, state->prev.pos.x);
		//if (delta_x != 0)
		//{
		//	debug::console({ "prev.x / current.x / delta_x / alpha: ", std::to_string(state->prev.pos.x), " / ", std::to_string(state->current.pos.x), " / ", std::to_string(delta_x), " / ", std::to_string(alpha) });
		//	//debug::console({ "state->current.pos.x + delta_x*alpha: ", std::to_string(state->current.pos.x), " + ", std::to_string(delta_x), " * ", std::to_string(alpha) });
		//}
		state->interpolated.x = state->current.pos.x + (delta_x)*alpha;
		//state->interpolated.x = state->current.pos.x*alpha + state->prev.pos.x*(1.0 - alpha);
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

	auto ship_id = 1;
	auto ship_collide = db->getComponentOfTypeForEntity<CollisionComponent>(ship_id);

	Rect ship_outer_box = ship_collide->outer_box;

	for (auto alien_collide : db->getComponentsOfTypeForEntityType<CollisionComponent>(E::ealien))
	{
		ship_outer_box.x = alien_collide->outer_box.x + playfield->getRelativePosX(alien_collide->outer_box.x, ship_outer_box.x);

		if (ship_collide && mathutil::areRectanglesIntersecting(ship_outer_box, alien_collide->outer_box))
		{
			ship_collide->is_collided = true;
			alien_collide->is_collided = true;
		}

		for (auto bullet_collide : db->getComponentsOfTypeForEntityType<CollisionComponent>(E::ebullet))
		{
			if (mathutil::areRectanglesIntersecting(bullet_collide->outer_box, alien_collide->outer_box))
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
		ship_outer_box.x = station_collide->outer_box.x + playfield->getRelativePosX(station_collide->outer_box.x, ship_outer_box.x);

		if (ship_collide && mathutil::areRectanglesIntersecting(ship_outer_box, station_collide->outer_box))
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