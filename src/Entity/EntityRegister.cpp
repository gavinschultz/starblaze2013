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