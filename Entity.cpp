#include "Entity.h"
#include <random>
#include <functional>
#include "Debug.h"
#include <ctime>

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
}

EntityRegister::EntityRegister()
{

}

EntityRegister::~EntityRegister()
{

}

World::World(int hill_count)
{
	auto rnd_hill_y = std::bind(std::uniform_real_distribution<double>{0.0, 1.0}, std::default_random_engine(time(0)));
	auto rnd_hill_x = std::bind(std::uniform_real_distribution<double>{0.0, this->w}, std::default_random_engine(time(0) - 1));
	auto rnd_type = std::bind(std::uniform_int_distribution<int>{0, 3}, std::default_random_engine(time(0) - 2));
	hills.reserve(hill_count);
	for (int i = 0; i < hill_count; i++)
	{
		Hill hill = Hill();
		hill.point.x = rnd_hill_x();
		hill.point.y = rnd_hill_y();
		hill.type = rnd_type();
		hills.push_back(hill);
	}
}