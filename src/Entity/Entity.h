#pragma once
#include <vector>
#include <memory>
#include <SDL.h>
#include "Phys.h"

class Entity 
{
protected:
	virtual const std::vector<Rect>* getBaseCollisionBoxes() const = 0;// { return nullptr; }
public:
	State2D prev_state;
	State2D current_state;
	Point2D alpha_pos;
	SDL_Rect bounding_box;
	std::vector<Rect> collision_boxes;
	int health{ 0 };
	double weight{ 1.0 };
	double max_lift{ 0.0 };
	double altitude{ 0.0 };
	bool is_active{ true };
	bool is_collided{ false };
	double age{ 0.0 };
	virtual double getDecelerationFactorX() const { return 1.0; }
	virtual double getDecelerationFactorY() const { return 1.0; }
	virtual void tick(double dt) { if (is_active) age += dt; }
	virtual void updateCollisionBoxes();

	bool isInactive();
};