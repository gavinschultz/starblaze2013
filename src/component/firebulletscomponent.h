#pragma once
#include <array>
#include "component_base.h"
#include "phys.h"

// Manages both the trigger to fire a bullet as well as the state of all the bullets themselves
class FireBulletsComponent : public Component
{
private:
	const static unsigned int BULLETS_MAX = 30;
	bool fire_{ false };
	int ammo_{ 128 };
	std::array<unsigned int, BULLETS_MAX> bullet_ids_;
	unsigned int next_available_index_{ 0 };
public:
	FireBulletsComponent() : Component() {}
	void fire();
	bool isFireRequired() const;
	void reset();
	void registerBullets(unsigned int lower_id);
	//void tick(float dt);
	unsigned int loadNextBullet();
	//std::array<unsigned int, BULLETS_MAX>& getBullets();
};