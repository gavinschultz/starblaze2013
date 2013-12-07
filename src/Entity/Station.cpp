#include "stdafx.h"
#include "Station.h"

#include "Bullet.h"
#include "Collidable.h"

class Station::impl
{
public:
	static const Rect base_outer_box;
	static const std::vector<Rect> Station::impl::base_collision_boxes;
};

const std::vector<Rect> Station::impl::base_collision_boxes{
	{ 0, 0, 32, 16 }
};
const Rect Station::impl::base_outer_box = { 0, 0, 32, 16 };

//Station::Station() : Entity(new NormalCollidable{ impl::base_outer_box, impl::base_collision_boxes }), pimpl{ new impl{} }
Station::Station() : Entity(), pimpl{ new impl{} }
{
	box = impl::base_outer_box;
}
Station::~Station(){}

const std::vector<Rect>* Station::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}