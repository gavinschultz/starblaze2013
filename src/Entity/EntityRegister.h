#pragma once
#include <memory>
#include <vector>

class Ship;
class Alien;
class Station;
class Entity;
class Bullet;

class EntityRegister
{
public:
	Ship* getShip() const;
	const std::vector<std::unique_ptr<Alien>>& getAliens() const;
	Station* getStation() const;
	const std::vector<std::unique_ptr<Bullet>>& getBullets() const;
	const std::vector<Entity*> getAll() const;
	void registerEntity(Ship* ship);
	void registerEntity(Alien* alien);
	void registerEntity(Station* station);
	void registerEntity(Bullet* bullet);
	EntityRegister();
	~EntityRegister();
private:
	std::unique_ptr<Station> _station;
	std::unique_ptr<Ship> _ship;
	std::vector<std::unique_ptr<Alien>> _aliens;
	std::vector<std::unique_ptr<Bullet>> _bullets;
	int _ship_count = 3;
};