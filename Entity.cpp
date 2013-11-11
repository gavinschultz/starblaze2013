#include "Entity.h"
#include <random>
#include <functional>
#include <array>
#include "Debug.h"
#include <ctime>

//Point2D::Point2D(double x, double y)
//{
//	this->x = x;
//	this->y = y;
//}
//
////Point2Di::Point2Di(int32_t x, int32_t)
//
//Vector2D::Vector2D(double x, double y)
//{
//	this->x = x;
//	this->y = y;
//}

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
	auto rnd_hill_y_channel = std::bind(std::uniform_int_distribution<int>{0, 3}, std::default_random_engine(time(0)));
	auto rnd_hill_x_channel = std::bind(std::uniform_int_distribution<int>{0, (int)(this->w / 8) - 1}, std::default_random_engine(time(0) - 1));
	auto rnd_type = std::bind(std::uniform_int_distribution<int>{0, 3}, std::default_random_engine(time(0) - 2));
	hills.reserve(hill_count);
	for (int i = 0; i < hill_count; i++)
	{
		Hill hill = Hill();
		hill.y_channel = rnd_hill_y_channel();
		hill.x_channel = rnd_hill_x_channel();
		hill.x = hill.x_channel * 8;
		hill.type = rnd_type();
		hills.push_back(hill);
	}
	for (auto& h : hills)
	{
		console_debug({"Hill X: ", std::to_string(h.x) });
	}
	hill = new Hill();
	hill->type = 3;
	hill->x = 25.0;
}