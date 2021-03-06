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
	int bullet_counter{ 0 };
	double bullet_reload_countdown{ 0.0 };
	static const std::vector<Rect> base_collision_boxes;
};
const std::vector<Rect> Ship::impl::base_collision_boxes{
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
	collision_boxes = std::vector<Rect>{ *getBaseCollisionBoxes() };
}
Ship::~Ship() {}

void Ship::fire()
{
	if (pimpl->bullet_reload_countdown != 0.0 && this->age < pimpl->bullet_reload_countdown)
		return;
	pimpl->bullet_reload_countdown = this->age + 0.2;
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

const std::vector<Rect>* Ship::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}

void Ship::updateCollisionBoxes()
{
	if (direction == ShipDirection::right)
	{
		Entity::updateCollisionBoxes();
	}
	else
	{
		auto base_rects = getBaseCollisionBoxes();
		for (unsigned int i = 0; i < base_rects->size(); i++)
		{
			collision_boxes[i].x = current_state.pos.x + (bounding_box.w - (base_rects->at(i).x + base_rects->at(i).w));
			collision_boxes[i].y = current_state.pos.y + (*base_rects)[i].y;
		}
	}
}