#pragma once
#include <SDL.h>
#include "Phys.h"

class Entity 
{
public:
	State2D prev_state;
	State2D current_state;
	Point2D alpha_pos;
	SDL_Rect bounding_box;
	int health;
	double weight;
	double max_lift;
	double altitude{ 0.0 };
	virtual double getDecelerationFactorX() const { return 1.0; }
	virtual double getDecelerationFactorY() const { return 1.0; }
};