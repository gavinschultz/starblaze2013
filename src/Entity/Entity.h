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
	int health{ 0 };
	double weight{ 0.0 };
	double max_lift{ 0.0 };
	double altitude{ 0.0 };
	bool is_active{ true };
	virtual double getDecelerationFactorX() const { return 1.0; }
	virtual double getDecelerationFactorY() const { return 1.0; }
};