#pragma once
#include <SDL.h>
#include "Phys.h"

class Entity 
{
public:
	State2D prev_state;
	State2D current_state;
	SDL_Rect bounding_box;
	int health;
	double weight;
	virtual double getDecelerationFactorX() const { return 1.0; }
	virtual double getDecelerationFactorY() const { return 1.0; }
};