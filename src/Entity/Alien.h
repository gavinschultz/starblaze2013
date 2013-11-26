#pragma once
#include "Entity.h"
#include "Phys.h"

class Alien : public Entity
{
public:
	State2D current_state;
	State2D prev_state;
	Point2D alpha_pos;
	SDL_Rect bounding_box{ { 0, 0, 16, 12 } };
	double altitude{ 0.0 };
};