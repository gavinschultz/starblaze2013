#pragma once
#include <memory>
#include <SDL.h>
#include "Phys.h"

enum class ShipDirection { left, right };

class Ship
{
public:
	State2D current_state;
	State2D prev_state;
	State2D alpha_state;
	double secs_at_slow_vel_x = 0.0;
	double secs_at_slow_vel_y = 0.0;
	SDL_Rect bounding_box;
	ShipDirection direction;
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