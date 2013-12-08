#pragma once

namespace physics
{
	void integrate(double delta_time, double dt);
	void interpolate(double alpha);
	void collisionDetection();
}