#pragma once
#include <SDL.h>
#include "Entity.h"
#include "Phys.h"

enum class ShipDirection { left, right };

class Ship : public Entity
{
public:
	Ship();
	Point2D alpha_pos;
	ShipDirection direction{ ShipDirection::right };
	double altitude{ 0.0 };
	unsigned int bullets{ 128 };
	int shields{ 100 };
	int radar{ 100 };
	int fuel{ 10000 };
	bool isGearDown();
	const double reverse_thrust_factor{ 1.6 };
	const double takeoff_speed{ 250.0 };
	const Vector2D max_thrust{ 400.0, 12.0 };
	const double max_velocity{ 20000.0 };
	const double max_lift{ 700.0 };
	double getDecelerationFactorX() const;
	double getDecelerationFactorY() const;
};