#include "stdafx.h"
#include "Bullet.h"

Bullet::Bullet()
{
	this->bounding_box = { 0, 0, 2, 1 };
	this->health = 1;
	this->weight = 0.01;
}