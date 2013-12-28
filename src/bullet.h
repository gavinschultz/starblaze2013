#pragma once
#include "component\thrustcomponent.h"
#include "component\temporalstatecomponent.h"
#include "component\physicalcomponent.h"
#include "component\collisioncomponent.h"
#include "component\lifetimecomponent.h"

class Bullet
{
public:
	Bullet() = default;
	~Bullet() = default;
	LifetimeComponent lifetime{ 0.7 };
	ThrustComponent thrust{ { 5000.0, 0.0 }, { 0.0 } };
	TemporalState2DComponent state;
	PhysicalComponent physical{ { 0.0, 0.0, 16.0, 8.0 }, 1.0 };
	CollisionComponent collision{ { 0.0, 0.0, 16.0, 8.0 }, {} };
};