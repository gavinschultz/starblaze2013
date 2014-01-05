#pragma once
#include <vector>
#include <memory>
#include <SDL.h>
#include "phys.h"
#include "component.h"

class Entity;

class Hill
{
public:
	int type{ 0 };
	int y_channel{ 0 };
	float x{ 0.0 };
	float distance_factor{ 0.0 };
};

class PlayField
{
public:
	PlayField(const Window& window, int hill_count = 100);
	const float w{ 1024.0 * 16 }; // 16 screens wide
	std::vector<Hill> hills;
	
	/**
	 * X & W determine the the "margins" in which the ship can sit, so aren't all that useful for other entities
	 * Y & H are more broadly useful for determining a valid vertical range for any object
	 */
	Rect boundaries;

	Rect getPlayArea(const Rect& entity_box) const;
	float getAltitude(float entity_pos_y, float entity_box_h) const;
	float getTakeoffSpeed();
	bool isRightOf(float x1, float x2);
	bool isLeftOf(float x1, float x2);
	float getAbsolutePosX(float entity_pos_x) const;
	float getRelativePosX(float ref_pos_x, float entity_pos_x) const;
};