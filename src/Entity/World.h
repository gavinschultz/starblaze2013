#pragma once
#include <vector>
#include <memory>
#include <SDL.h>
#include "Hill.h"
#include "Phys.h"

class Entity;

class World
{
public:
	World(int hill_count = 100);
	const double w{ 256.0 * 16 }; // equivalent to 16 screens
	std::vector<Hill> hills;
	//SDL_Rect ship_limits{ { 48, 0, 160, 144 } };	// square
	//SDL_Rect ship_limits{ { 48, 0, 248, 144 } };	// 1366 x 768 - short edges
	Rect ship_limits{ 56, 0, 232, 144 };	// 1366 x 768 - far edges
	Rect getPlayArea(const Rect& entity_box);
	double getAltitude(const Entity& entity);
};

extern std::unique_ptr<World> world;