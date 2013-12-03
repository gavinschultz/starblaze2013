#pragma once
#include <vector>
#include <memory>
#include "Entity.h"
#include "Phys.h"

class DebrisItem
{
public:
	double lifetime{ 0.0 };
	State2D current_state;
	State2D prev_state;
	Point2D alpha_pos;
};

class Debris
{
private:
	std::vector<std::unique_ptr<DebrisItem>> _items;
public:
	Debris(const State2D& origin);
	~Debris();
	const std::vector<std::unique_ptr<DebrisItem>>& getItems() const;
};