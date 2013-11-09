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
	Point2D prev_alpha_pos;
	double secs_at_slow_vel_x{ 0.0 };
	double secs_at_slow_vel_y{ 0.0 };
	SDL_Rect bounding_box;
	ShipDirection direction;
	double altitude{ 0.0 };
	bool isGearDown();
	const double takeoff_speed{ 250.0 };
	double max_thrust{ 1500.0 };
	double weight{ 1.0 };

};

class Hill
{
public:
	int type;
	int x_channel;
	int y_channel;
	double x{ 0.0 };
};

class World
{
public:
	World(int hill_count = 100);
	double w{ 1024.0 / 2 };	// 1024 * 16
	std::vector<Hill> hills;
	Hill* hill;
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