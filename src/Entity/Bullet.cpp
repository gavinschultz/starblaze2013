#include "stdafx.h"
#include "Bullet.h"
#include "Alien.h"
#include "Collidable.h"

class Bullet::impl
{
public:
	static const std::vector<Rect> base_collision_boxes;
	double age{ 0.0 };
};
const std::vector<Rect> Bullet::impl::base_collision_boxes{
	{ 0, 0, 4, 2 }
};

Bullet::Bullet() : pimpl{ new impl{} }
{
	this->is_active = false;
	this->box = { 0, 0, 4, 2 };
	this->attrib.health = 1;
	this->attrib.weight = 0.5;
	this->attrib.max_lift = 0.0;
	this->_collidable = std::make_unique<Collidable>(new NormalCollidable(box, *getBaseCollisionBoxes(), { defaultBulletToAlienCollider }));
	this->deceleration_factor = { 0.0, 0.0 };
}

Bullet::~Bullet() {}

void Bullet::tick(double dt)
{
	if (is_active)
		pimpl->age += dt;
	if (pimpl->age > 0.7)
		this->is_active = false;
}

void Bullet::reset()
{
	pimpl->age = 0.0;
	is_active = true;
}

const std::vector<Rect>* Bullet::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}