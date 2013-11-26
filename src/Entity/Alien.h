#pragma once
#include "Entity\Entity.h"
#include "Phys.h"

class Alien : public Entity
{
public:
	State2D current_state;
	State2D prev_state;
	Point2D alpha_pos;
	SDL_Rect bounding_box;
	double altitude{ 0.0 };
};