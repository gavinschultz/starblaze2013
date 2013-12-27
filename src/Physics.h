#pragma once

class PhysicsSystem
{
public:
	void update(double dt);
	void interpolate(double alpha);
	void collide();
	double dt{ 1.0 / 120.0 };
};