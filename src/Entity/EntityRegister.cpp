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

EntityRegister::EntityRegister()
{

}

EntityRegister::~EntityRegister()
{

}