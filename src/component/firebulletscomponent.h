#pragma once
#include <array>
#include "component_base.h"
#include "phys.h"

class Bullet
{
public:
	Rect rect;
	double age;
	bool is_active;
};

// Manages both the trigger to fire a bullet as well as the state of all the bullets themselves
class FireBulletsComponent : public Component
{
private:
	bool fire_{ false };
	int ammo_{ 128 };
	std::array<Bullet, 30> bullets_;
public:
	FireBulletsComponent() : Component() {}
	void fire();
	bool isFireRequired();
	void reset();
	void tick(double dt);
	const std::vector<Bullet*> getActiveBullets();
};