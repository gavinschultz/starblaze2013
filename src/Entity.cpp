#include "stdafx.h"
#include "Entity.h"
#include <random>
#include <functional>
#include <array>
#include "Debug.h"
#include "Entity\Alien.h"
#include <ctime>

void EntityRegister::registerEntity(Ship* ship)
{
	_ship = std::shared_ptr<Ship>{ship};
}

void EntityRegister::registerEntity(Alien* alien)
{
	auto p_alien = std::make_shared<Alien>(*alien);
	_aliens.push_back(p_alien);
}

void EntityRegister::registerEntity(Station* station)
{
	_station = std::unique_ptr<Station>{station};
}

Ship* EntityRegister::getShip()
{
	return _ship.get();
}

const int EntityRegister::getShipCount()
{
	return _ship_count;
}

Alien* EntityRegister::getAlien()
{
	return _aliens[0].get();
}

Station* EntityRegister::getStation()
{
	return _station.get();
}

EntityRegister::EntityRegister()
{

}

EntityRegister::~EntityRegister()
{

}

World::World(int hill_count)
{
	auto rnd_hill_y_channel = std::bind(std::uniform_int_distribution<int>{0, 3}, std::default_random_engine((uint32_t)time(0)));
	auto rnd_hill_x_channel = std::bind(std::uniform_int_distribution<int>{0, (int)(this->w / 8) - 1}, std::default_random_engine((uint32_t)time(0) - 1));
	auto rnd_type = std::bind(std::uniform_int_distribution<int>{0, 3}, std::default_random_engine((uint32_t)time(0) - 2));
	const std::array<double, 4> y_channel_speeds = { 0.1, 0.25, 0.5, 0.75 };
	hills.reserve(hill_count);
	for (int i = 0; i < hill_count; i++)
	{
		Hill hill = Hill();
		hill.y_channel = rnd_hill_y_channel();

		int x_channel = rnd_hill_x_channel();
		hill.x = x_channel * 8;

		if (hill.x > this->w * y_channel_speeds[hill.y_channel])
			hill.x = hill.x * y_channel_speeds[hill.y_channel];

		hill.distance_factor = y_channel_speeds[hill.y_channel];
		hill.type = rnd_type();
		hills.push_back(hill);
	}
}