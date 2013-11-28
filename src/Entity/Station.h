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
	Station();
	StationType station_type;
};