#pragma once
#include <SDL.h>
#include <memory>
#include "Entity.h"
#include "Phys.h"

enum StationType
{
	Fuel = 0,
	Repair
};

class Station : public Entity
{
private:
	class impl; std::unique_ptr<impl> pimpl;
protected:
	const std::vector<Rect>* Station::getBaseCollisionBoxes() const;
public:
	Station();
	~Station();
	StationType station_type;
	bool is_docked{ false };
};