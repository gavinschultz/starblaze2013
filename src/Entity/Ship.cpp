#include "stdafx.h"
#include "Ship.h"
#include <cmath>
#include <array>
#include <vector>
#include "Bullet.h"

#include "Game.h"

class Ship::impl
{
public:
	static int bullet_counter;
	static const std::vector<SDL_Rect> base_collision_boxes;
};
int Ship::impl::bullet_counter{ 0 };
const std::vector<SDL_Rect> Ship::impl::base_collision_boxes{
	{ 0, 1, 4, 7 },
	{ 4, 2, 2, 6 },
	{ 6, 3, 2, 5 },
	{ 8, 4, 18, 4 },
	{ 26, 5, 6, 3 }
};

Ship::Ship() : pimpl{ new impl{} }
{
	bounding_box = { 0, 0, 32, 8 };
	weight = 1.0;
	max_lift = 700.0;
	collision_boxes = std::vector<SDL_Rect>{ *getBaseCollisionBoxes() };
}
Ship::~Ship() {}

void Ship::fire()
{
	auto& bullets = game->entity_register.getBullets();
	auto& fired_bullet = bullets[pimpl->bullet_counter];
	fired_bullet->is_active = true;
	fired_bullet->age = 0;
	fired_bullet->current_state.pos.x = (this->direction == ShipDirection::left ? this->current_state.pos.x : this->current_state.pos.x + this->bounding_box.w);
	fired_bullet->current_state.pos.y = this->current_state.pos.y + 6;
	fired_bullet->current_state.vel.x = (this->direction == ShipDirection::left ? -std::abs(this->current_state.vel.x) - 400.0 : std::abs(this->current_state.vel.x) + 400.0);
	fired_bullet->current_state.thrust.x = (this->direction == ShipDirection::left ? -500.0 : 500.0);
	fired_bullet->prev_state = fired_bullet->current_state;
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

const std::vector<SDL_Rect>* Ship::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}