#include "stdafx.h"
#include "Ship.h"
#include <cmath>
#include <array>
#include "Bullet.h"

#include "Game.h"

class Ship::impl
{
public:
	static int bullet_counter;
};
int Ship::impl::bullet_counter{ 0 };

Ship::Ship() : pimpl{ new impl{} }
{
	bounding_box = { 0, 0, 32, 8 };
	weight = 1.0;
	max_lift = 700.0;
}
Ship::~Ship() {}

void Ship::fire()
{
	auto& bullets = game->entity_register.getBullets();
	auto& fired_bullet = bullets[pimpl->bullet_counter];
	fired_bullet->is_active = true;
	fired_bullet->current_state.pos.x = this->current_state.pos.x;
	fired_bullet->current_state.pos.y = this->current_state.pos.y;
	fired_bullet->current_state.vel.x = (this->direction == ShipDirection::left ? -std::abs(this->current_state.vel.x) - 400.0 : std::abs(this->current_state.vel.x) + 400.0);
	fired_bullet->current_state.thrust.x = (this->direction == ShipDirection::left ? -500.0 : 500.0);
	pimpl->bullet_counter = (pimpl->bullet_counter + 1) % bullets.size();
}

double Ship::getDecelerationFactorX() const
{
	if (std::abs(current_state.vel.x) > 200 || std::abs(current_state.thrust.x) > 0)
		return 0.4;
	else if (std::abs(current_state.vel.x) > 50) // slow more when already slow and no thrust
		return 1.1;
	else
		return 2.3;
}

double Ship::getDecelerationFactorY() const
{
	return 4.0;
}