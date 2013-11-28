#include "stdafx.h"
#include "EntityRegister.h"
#include "Entity\Alien.h"
#include "Entity\Hill.h"
#include "Entity\Station.h"
#include "Entity\Ship.h"

void EntityRegister::registerEntity(Ship* ship)
{
	_ship = std::unique_ptr<Ship>{ship};
}

void EntityRegister::registerEntity(Alien* alien)
{
	_aliens.push_back(std::unique_ptr<Alien>{alien});
}

void EntityRegister::registerEntity(Station* station)
{
	_station = std::unique_ptr<Station>{station};
}

Ship* EntityRegister::getShip() const
{
	return _ship.get();
}

const std::vector<std::unique_ptr<Alien>>& EntityRegister::getAliens() const
{
	return _aliens;
}

Station* EntityRegister::getStation() const
{
	return _station.get();
}

const std::vector<Entity*> EntityRegister::getAll() const
{
	// probably more efficient ways of doing this
	std::vector<Entity*> all;
	all.push_back(_ship.get());
	all.push_back(_station.get());
	for (auto& alien : _aliens)
	{
		all.push_back(alien.get());
	}
	return all;
}

EntityRegister::EntityRegister()
{

}

EntityRegister::~EntityRegister()
{

}