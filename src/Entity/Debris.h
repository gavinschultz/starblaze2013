#pragma once
#include <vector>
#include <memory>
#include "Entity.h"
#include "Phys.h"

class DebrisItem
{
public:
	double lifetime{ 0.0 };
	TemporalState2D state;
	bool isAlive() { return lifetime > 0.0; }
};

class Debris
{
private:
	std::vector<std::unique_ptr<DebrisItem>> _items;
public:
	Debris(const State2D& origin);
	~Debris();
	const std::vector<std::unique_ptr<DebrisItem>>& getItems() const;
	void tick(double dt);
};