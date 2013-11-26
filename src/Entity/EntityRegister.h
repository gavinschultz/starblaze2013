#pragma once
#include <memory>
#include <vector>

class Ship;
class Alien;
class Station;

class EntityRegister
{
public:
	Ship* getShip();
	Alien* getAlien();
	Station* getStation();
	const int getShipCount();
	void registerEntity(Ship* ship);
	void registerEntity(Alien* alien);
	void registerEntity(Station* station);
	EntityRegister();
	~EntityRegister();
private:
	std::unique_ptr<Station> _station;
	std::unique_ptr<Ship> _ship;
	std::vector<std::shared_ptr<Alien>> _aliens;
	int _ship_count = 3;
};