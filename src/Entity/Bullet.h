#pragma once
#include "Entity.h"

class Bullet : public Entity
{
public:
	Bullet();
	~Bullet();
	double getDecelerationFactorX() const;
};