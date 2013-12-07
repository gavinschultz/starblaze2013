#pragma once
#include <vector>
#include <memory>
#include <SDL.h>
#include "Phys.h"
#include "PhysicalAttributes.h"
//#include "Collidable.h"

class Collidable;

class Entity 
{
private:
	std::unique_ptr<Collidable> _collidable;
protected:
	Vector2D deceleration_factor{ 1.0, 1.0 };
public:
	//Entity(Collidable* collidable);
	Entity();
	~Entity();
	TemporalState2D state;
	Rect box;
	PhysicalAttributes attrib;
	virtual void tick(double dt) {};
	bool is_active{ true };
	virtual Vector2D getDecelerationFactor() { return deceleration_factor; }
	virtual void damage(int points) { attrib.health -= points; }

	// Collisions
	Collidable& getCollidable() const;
	virtual void updateCollisionBoxes();
};