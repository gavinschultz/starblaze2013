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

void FireBulletsComponent::tick(double dt)
{
	for (auto& bullet : bullets_)
	{
		bullet.lifetime.tick(dt);
	}
}

std::array<Bullet, FireBulletsComponent::BULLETS_MAX>& FireBulletsComponent::getBullets()
{
	return bullets_;
}

Bullet* FireBulletsComponent::loadNext()
{
	auto bullet = &bullets_[next_available_index];
	bullet->lifetime.reset();
	next_available_index = ++next_available_index % BULLETS_MAX;
	return bullet;
}