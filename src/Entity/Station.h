#pragma once
#include <SDL.h>
#include "Entity.h"
#include "Phys.h"

enum StationType
{
	Fuel = 0,
	Repair
};

class Station : public Entity
{
public:
	int station_type{ 0 };
	Point2D pos{ 0.0, 0.0 };
	SDL_Rect bounding_box{ { 0, 0, 32, 16 } };
};