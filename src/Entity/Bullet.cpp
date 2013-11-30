#include "stdafx.h"
#include "Bullet.h"

Bullet::Bullet()
{
	this->is_active = false;
	this->bounding_box = { 0, 0, 2, 2 };
	this->health = 1;
	this->weight = 0.5;
	this->max_lift = 0.0;
}

Bullet::~Bullet() {}

double Bullet::getDecelerationFactorX() const
{
	return 0.0;
}