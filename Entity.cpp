#include "Entity.h"

Point2D::Point2D(double x, double y)
{
	this->x = x;
	this->y = y;
}

Vector2D::Vector2D(double x, double y)
{
	this->x = x;
	this->y = y;
}

Ship* EntityRegister::getShip()
{
	return _ship.get();
}

const int EntityRegister::getShipCount()
{
	return _ship_count;
}

void EntityRegister::registerEntity(Ship* ship)
{
	_ship = std::shared_ptr<Ship>{ship};
	_ship->bounding_box = { 0, 0, 32 * 4, 8 * 4 };	// TODO: scaling real-time
}

EntityRegister::EntityRegister()
{

}

EntityRegister::~EntityRegister()
{

}