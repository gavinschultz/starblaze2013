#include "stdafx.h"
#include <algorithm>
#include <functional>
#include "EntityRegister.h"
#include "Entity\Alien.h"
#include "Entity\Hill.h"
#include "Entity\Station.h"
#include "Entity\Ship.h"
#include "Entity\Bullet.h"
#include "Entity\Debris.h"

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

void EntityRegister::registerEntity(Bullet* bullet)
{
	_bullets.push_back(std::unique_ptr<Bullet>{bullet});
}

void EntityRegister::registerEntity(Debris* debris)
{
	_debris.push_back(std::unique_ptr<Debris>{debris});
}

void EntityRegister::removeInactives()
{
	if (!_ship->is_active) 
		_ship = nullptr;
	_aliens.erase(std::remove_if(_aliens.begin(), _aliens.end(), std::bind(&Entity::is_active, std::placeholders::_1)), _aliens.end()); //  broken
	if (_station->is_active)
		_station = nullptr;
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

const std::vector<std::unique_ptr<Bullet>>& EntityRegister::getBullets() const
{
	return _bullets;
}

const std::vector<std::unique_ptr<Debris>>& EntityRegister::getDebris() const
{
	return _debris;
}

const std::vector<Entity*> EntityRegister::getAll() const
{
	// probably more efficient ways of doing this
	std::vector<Entity*> all;
	all.push_back(_ship.get());
	all.push_back(_station.get());
	for (auto& alien : _aliens)
		all.push_back(alien.get());
	for (auto& bullet : _bullets)
		all.push_back(bullet.get());
	return all;
}

const std::vector<Entity&> EntityRegister::getAllActive() const
{
	std::vector<Entity&> active;
	active.push_back(*_ship.get());
	active.push_back(*_station.get());
	for (auto& alien : _aliens)
		active.push_back(*alien.get());
	for (auto& bullet : _bullets)
		active.push_back(*bullet.get());
	return active;
}