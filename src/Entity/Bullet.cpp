#include "stdafx.h"
#include "Bullet.h"
#include "Alien.h"
#include "Collidable.h"

class Bullet::impl
{
public:
	static const std::vector<Rect> base_collision_boxes;
	static const Rect base_outer_box;
	double age{ 0.0 };
};
const std::vector<Rect> Bullet::impl::base_collision_boxes{
	{ 0, 0, 4, 2 }
};
const Rect Bullet::impl::base_outer_box{ 0, 0, 4, 2 };

//Bullet::Bullet() : Entity(new NormalCollidable{impl::base_outer_box, impl::base_collision_boxes, { defaultBulletToAlienCollider.get() }}), pimpl{ new impl{} }
Bullet::Bullet() : Entity(), pimpl{ new impl{} }
{
	this->is_active = false;
	this->box = impl::base_outer_box;
	this->attrib.health = 1;
	this->attrib.weight = 0.5;
	this->attrib.max_lift = 0.0;
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