#pragma once
#include <vector>
#include <memory>
#include "Hill.h"

class World
{
public:
	World(int hill_count = 100);
	const double w{ 256.0 * 16 }; // equivalent to 16 screens
	std::vector<Hill> hills;
};

extern std::unique_ptr<World> world;