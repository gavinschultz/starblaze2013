#include "stdafx.h"
#include "firebulletscomponent.h"

namespace
{
	const float BULLET_MAX_AGE = 0.7;
}

void FireBulletsComponent::fire()
{
	if (current_ammo_ == 0)
		return;
	if (reload_required_)
		return;
	fire_ = true;
	reload_required_ = true;
	current_ammo_--;
}

bool FireBulletsComponent::isFireRequired() const
{ 
	return fire_;
}

bool FireBulletsComponent::isReloadRequired() const
{
	return reload_required_;
}

void FireBulletsComponent::reload()
{
	reload_required_ = false;
}

void FireBulletsComponent::reset()
{ 
	fire_ = false; 
}

void FireBulletsComponent::registerBullets(unsigned int lower_id)
{
	for (int i = 0; i < BULLETS_MAX; i++)
	{
		bullet_ids_[i] = lower_id + i;
	}
}

//void FireBulletsComponent::tick(float dt)
//{
//	for (auto& bullet : bullets_)
//	{
//		bullet.lifetime.tick(dt);
//	}
//}
//
//std::array<Bullet, FireBulletsComponent::BULLETS_MAX>& FireBulletsComponent::getBullets()
//{
//	return bullets_;
//}

unsigned int FireBulletsComponent::loadNextBullet()
{
	auto bullet_id = bullet_ids_[next_available_index_];
	next_available_index_ = ++next_available_index_ % BULLETS_MAX;
	return bullet_id;
}

unsigned int FireBulletsComponent::maxAmmoCount() const
{
	return max_ammo_;
}

unsigned int FireBulletsComponent::currentAmmoCount() const
{
	return current_ammo_;
}