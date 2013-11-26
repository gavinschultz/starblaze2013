#pragma once
#include <SDL.h>
#include "Entity\Entity.h"
#include "Phys.h"

enum class ShipDirection { left, right };

class Ship : public Entity
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