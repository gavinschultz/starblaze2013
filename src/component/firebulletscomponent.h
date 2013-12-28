#pragma once
#include <array>
#include "component_base.h"
#include "bullet.h"
#include "phys.h"

// Manages both the trigger to fire a bullet as well as the state of all the bullets themselves
class FireBulletsComponent : public Component
{
private:
	const static unsigned int BULLETS_MAX = 30;
	bool fire_{ false };
	int ammo_{ 128 };
	std::array<Bullet, BULLETS_MAX> bullets_;
	unsigned int next_available_index{ 0 };
public:
	FireBulletsComponent() : Component() {}
	void fire();
	bool isFireRequired() const;
	void reset();
	void tick(double dt);
	Bullet* loadNext();
	std::array<Bullet, BULLETS_MAX>& getBullets();
};