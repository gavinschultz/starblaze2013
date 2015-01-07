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
	bool reload_required_{ false };
	unsigned int current_ammo_;
	unsigned int max_ammo_;
	std::array<unsigned int, BULLETS_MAX> bullet_ids_;
	unsigned int next_available_index_{ 0 };
public:
	FireBulletsComponent(int ammo_count = 128) : Component(), current_ammo_(ammo_count), max_ammo_(ammo_count) {}
	void fire();
	
	// indicates to the physics system that a bullet needs to be fired
	bool isFireRequired() const;

	// indicates to the input system that the chamber is empty and the user needs to reload
	bool isReloadRequired() const;
	void reload();
	void reset();
	void registerBullets(unsigned int lower_id);
	//void tick(float dt);
	unsigned int loadNextBullet();
	//std::array<unsigned int, BULLETS_MAX>& getBullets();
	unsigned int maxAmmoCount() const;
	unsigned int currentAmmoCount() const;
};