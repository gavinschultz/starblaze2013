#pragma once
#include "component_base.h"

class PhysicalComponent;
class TemporalState2DComponent;

class RadarTrackableComponent : public Component
{
public:
	RadarTrackableComponent(PhysicalComponent* phys = nullptr, TemporalState2DComponent* state = nullptr) : Component(C::radartrackable), phys{ phys }, state{ state } {}
	PhysicalComponent* phys;
	TemporalState2DComponent* state;
};