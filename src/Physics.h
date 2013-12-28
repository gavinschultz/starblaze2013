#pragma once

class TemporalState2DComponent;
class ThrustComponent;
class PhysicalComponent;
class HorizontalOrientComponent;
class CollisionComponent;

class PhysicsSystem
{
private:
	void update_impl(TemporalState2DComponent* state, ThrustComponent* thrust, PhysicalComponent* physical, HorizontalOrientComponent* horient, CollisionComponent* collision);
public:
	void update(double dt);
	void interpolate(double alpha);
	void collide();
	double dt{ 1.0 / 120.0 };
};