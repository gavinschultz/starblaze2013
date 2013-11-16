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
	int alpha_loop_count;
	SDL_Rect bounding_box;
	ShipDirection direction;
	double altitude{ 0.0 };
	bool isGearDown();
	const double takeoff_speed{ 250.0 };
	double max_thrust{ 400.0 };
	double weight{ 1.0 };
	double max_velocity{ 20000.0 };
	double max_lift{ 800.0 };
};

class Hill
{
public:
	int type{ 0 };
	//int x_channel;
	int y_channel{ 0 };
	double x{ 0.0 };
	double distance_factor{ 0.0 };
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
	const int getShipCount();
	void registerEntity(Ship* ship);
	EntityRegister();
	~EntityRegister();
private:
	std::shared_ptr<Ship> _ship;
	int _ship_count = 3;
};

extern std::unique_ptr<World> world;