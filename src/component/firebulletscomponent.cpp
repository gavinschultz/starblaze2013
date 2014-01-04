#include "stdafx.h"
#include "firebulletscomponent.h"

namespace
{
	const double BULLET_MAX_AGE = 0.7;
}

void FireBulletsComponent::fire()
{
	if (ammo_ <= 0)
		return;
	fire_ = true;
	ammo_--;
}

bool FireBulletsComponent::isFireRequired() const
{ 
	return fire_;
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

//void FireBulletsComponent::tick(double dt)
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