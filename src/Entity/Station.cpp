#include "stdafx.h"
#include "Station.h"

class Station::impl
{
public:
	static const std::vector<SDL_Rect> Station::impl::base_collision_boxes;
};

const std::vector<SDL_Rect> Station::impl::base_collision_boxes{
	{ 0, 0, 32, 16 }
};

Station::Station() : pimpl{ new impl{} }
{
	bounding_box = { 0, 0, 32, 16 };
	collision_boxes = std::vector<SDL_Rect>{ *getBaseCollisionBoxes() };
}
Station::~Station(){}

const std::vector<SDL_Rect>* Station::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}