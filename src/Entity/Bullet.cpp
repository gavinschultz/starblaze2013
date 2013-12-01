#include "stdafx.h"
#include "Bullet.h"

class Bullet::impl
{
public:
	static const std::vector<SDL_Rect> base_collision_boxes;
};
const std::vector<SDL_Rect> Bullet::impl::base_collision_boxes{
	{ 0, 0, 4, 2 }
};

Bullet::Bullet() : pimpl{ new impl{} }
{
	this->is_active = false;
	this->bounding_box = { 0, 0, 4, 2 };
	this->health = 1;
	this->weight = 0.5;
	this->max_lift = 0.0;
	collision_boxes = std::vector<SDL_Rect>{ *getBaseCollisionBoxes() };
}

Bullet::~Bullet() {}

double Bullet::getDecelerationFactorX() const
{
	return 0.0;
}

void Bullet::tick(double dt)
{
	Entity::tick(dt);
	if (age > 0.7)
		this->is_active = false;
}

const std::vector<SDL_Rect>* Bullet::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}