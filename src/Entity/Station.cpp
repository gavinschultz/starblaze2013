#include "stdafx.h"
#include "Station.h"

#include "Bullet.h"

class Station::impl
{
public:
	static const std::vector<Rect> Station::impl::base_collision_boxes;
};

const std::vector<Rect> Station::impl::base_collision_boxes{
	{ 0, 0, 32, 16 }
};

Station::Station() : pimpl{ new impl{} }
{
	box = { 0, 0, 32, 16 };
	_collidable = std::make_unique<Collidable>(new NormalCollidable(box, *getBaseCollisionBoxes(), { &typeid(Bullet) }));
}
Station::~Station(){}

const std::vector<Rect>* Station::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}