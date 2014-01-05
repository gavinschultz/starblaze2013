#pragma once

class TemporalState2DComponent;
class ThrustComponent;
class PhysicalComponent;
class HorizontalOrientComponent;
class CollisionComponent;

class PhysicsSystem
{
public:
	void update(float dt);
	void interpolate(float alpha);
	void collide();
	float dt{ 1.0 / 120.0 };
};