#include "stdafx.h"
#include "Ship.h"
#include <cmath>
#include <array>
#include <vector>
#include <memory>
#include "Bullet.h"
#include "Alien.h"
#include "Station.h"
#include "World.h"
#include "Game.h"
#include "Collidable.h"

class Ship::impl
{
public:
	const int RELOAD_TIME = 0.2;
	bool isReloading()
	{
		return (reload_countdown > 0.0);
	}
	void resetReloader()
	{
		reload_countdown = RELOAD_TIME;
	}
	Bullet& getNextBullet()
	{
		auto& bullet_pool = game->entity_register.getBullets();
		bullet_counter = (bullet_counter + 1) % bullet_pool.size();	// wrap-around counter

		auto& bullet = *bullet_pool[bullet_counter];
		bullet.reset();
		return bullet;
	}
	double age{ 0.0 };
	int bullet_counter{ 0 };
	double reload_countdown{ 0.5 };
	static const Rect base_outer_box;
	static const std::vector<Rect> base_collision_boxes;
	//static const std::vector<std::unique_ptr<Collider>> colliders;
};
const Rect Ship::impl::base_outer_box{ 0, 0, 32, 8 };
const std::vector<Rect> Ship::impl::base_collision_boxes{
	{ 0, 1, 4, 7 },
	{ 4, 2, 2, 6 },
	{ 6, 3, 2, 5 },
	{ 8, 4, 18, 4 },
	{ 26, 5, 6, 3 }
};

//Ship::Ship() : Entity(new NormalCollidable{ impl::base_outer_box, impl::base_collision_boxes, { defaultShipToAlienCollider.get(), defaultShipToStationCollider.get() } }), pimpl{ new impl{} }
Ship::Ship() : Entity(), pimpl{ new impl{} }
{
	box = impl::base_outer_box;
	attrib.weight = 110.0;
	attrib.max_lift = 700.0;
}
Ship::~Ship() {}

//const std::vector<std::unique_ptr<Collider>>& Ship::getColliders() { return pimpl->colliders; }

void Ship::fire()
{
	if (pimpl->isReloading())
		return;
	pimpl->resetReloader();
	auto& fired_bullet = pimpl->getNextBullet();
	fired_bullet.state.current.pos.x = (this->direction == ShipDirection::left ? this->state.current.pos.x : this->state.current.pos.x + this->box.w);
	fired_bullet.state.current.pos.y = this->state.current.pos.y + 6;
	fired_bullet.state.current.vel.x = (this->direction == ShipDirection::left ? -std::abs(this->state.current.vel.x) - 400.0 : std::abs(this->state.current.vel.x) + 400.0);
	fired_bullet.state.current.thrust.x = (this->direction == ShipDirection::left ? -500.0 : 500.0);
	fired_bullet.state.prev = fired_bullet.state.current;
}

double Ship::getDecelerationFactorX() const
{
	if (std::abs(state.current.vel.x) > 200 || std::abs(state.current.thrust.x) > 0)
		return 0.4;
	else if (std::abs(state.current.vel.x) > 50) // slow more when already slow and no thrust
		return 1.1;
	else
		return 2.3;
}

double Ship::getDecelerationFactorY() const
{
	return 4.0;
}

const std::vector<Rect>* Ship::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}

void Ship::updateCollisionBoxes(Point2D pos)
{
	//this->getCollidable().updateCollisionBoxes(pos, (direction == ShipDirection::left));
}

bool Ship::isGearDown() const
{
	double altitude = world->getAltitude(*this);
	return (altitude == 0.0 && std::abs(this->state.current.vel.x) < this->takeoff_speed);
}