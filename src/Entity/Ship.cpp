#include "stdafx.h"
#include "Ship.h"
#include <cmath>

Ship::Ship()
{
	bounding_box = { 0, 0, 32, 8 };
	weight = 1.0;
	max_lift = 700.0;
}

double Ship::getDecelerationFactorX() const
{
	if (std::abs(current_state.vel.x) > 200 || std::abs(current_state.thrust.x) > 0)
		return 0.4;
	else if (std::abs(current_state.vel.x) > 50) // slow more when already slow and no thrust
		return 1.1;
	else
		return 2.3;
}

double Ship::getDecelerationFactorY() const
{
	return 4.0;
}