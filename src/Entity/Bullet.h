#pragma once
#include "Entity.h"

class Bullet : public Entity
{
private:
	class impl; std::unique_ptr<impl> pimpl;
protected:
	const std::vector<Rect>* Bullet::getBaseCollisionBoxes() const;
public:
	Bullet();
	~Bullet();
	double getDecelerationFactorX() const;
	void tick(double dt);
	void reset();
};