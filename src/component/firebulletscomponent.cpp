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

bool FireBulletsComponent::isFireRequired()
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
		if (!bullet.is_active)
			continue;

		bullet.age += dt;
		if (bullet.age > BULLET_MAX_AGE)
		{
			bullet.is_active = false;
		}
	}
}

const std::vector<Bullet*> FireBulletsComponent::getActiveBullets()
{
	std::vector<Bullet*> active_bullets;
	active_bullets.reserve(bullets_.size());
	for (auto& bullet : bullets_)
	{
		if (bullet.is_active)
			active_bullets.push_back(&bullet);
	}
	return active_bullets;
}