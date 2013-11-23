#pragma once
#include <memory>
#include <SDL.h>
#include "Phys.h"
#include <vector>

enum class ShipDirection { left, right };

class Ship
{
public:
	State2D current_state;
	State2D prev_state;
	Point2D alpha_pos;
	SDL_Rect bounding_box;
	ShipDirection direction;
	double altitude{ 0.0 };
	unsigned int bullets{ 128 };
	int shields{ 100 };
	int radar{ 100 };
	int fuel{ 10000 };
	bool isGearDown();
	const double reverse_thrust_factor{ 1.6 };
	const double takeoff_speed{ 250.0 };
	const Vector2D max_thrust{ 400.0, 12.0 };
	const double weight{ 1.0 };
	const double max_velocity{ 20000.0 };
	const double max_lift{ 700.0 };
};

class Alien
{
public:
	State2D current_state;
	State2D prev_state;
	Point2D alpha_state;
	SDL_Rect bounding_box;
	double altitude{ 0.0 };
};

class Hill
{
public:
	int type{ 0 };
	int y_channel{ 0 };
	double x{ 0.0 };
	double distance_factor{ 0.0 };
};

class Station
{
public:
	int station_type{ 0 };
	Point2D pos{ 0.0, 0.0 };
};

class World
{
public:
	World(int hill_count = 100);
	const double w{ 256.0 * 16 }; // equivalent to 16 screens
	std::vector<Hill> hills;
};

class EntityRegister
{
public:
	Ship* getShip();
	Alien* getAlien();
	const int getShipCount();
	void registerEntity(Ship* ship);
	std::shared_ptr<Alien> registerEntity(Alien* alien);
	std::shared_ptr<Station> registerEntity(Station* station);
	EntityRegister();
	~EntityRegister();
private:
	std::shared_ptr<Ship> _ship;
	std::vector<std::shared_ptr<Alien>> _aliens;
	int _ship_count = 3;
};

extern std::unique_ptr<World> world;