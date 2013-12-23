#pragma once
#include "component_base.h"

enum StationType
{
	fuel = 0,
	repair
};

class StationComponent : public Component
{
public:
	StationComponent(StationType station_type) : Component(C::station), station_type{ station_type } {}
	StationType station_type;
	bool is_docked{ false };
};