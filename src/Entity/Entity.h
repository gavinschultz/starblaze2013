#pragma once
#include <vector>
#include <memory>
#include <SDL.h>
#include "Phys.h"
#include "PhysicalAttributes.h"
#include "Collidable.h"

class Entity 
{
protected:
	Vector2D deceleration_factor{ 1.0, 1.0 };
	std::unique_ptr<Collidable> _collidable;
public:
	Entity(Collidable* collidable) : _collidable(collidable) {}
	TemporalState2D state;
	Rect box;
	PhysicalAttributes attrib;
	virtual void tick(double dt) {};
	bool is_active{ true };
	virtual Vector2D getDecelerationFactor() { return deceleration_factor; }
	virtual void damage(int points) { attrib.health -= points; }

	// Collisions
	virtual const std::vector<std::unique_ptr<Collider>>& getColliders() = 0;
	bool canCollideWith(const Entity& entity) const { return _collidable->canCollideWith(entity); }
	Collidable& getCollidable() const { return *_collidable; }
	virtual const std::vector<Rect>& getCollisionBoxes() const { return _collidable->getCollisionBoxes(); }
	virtual void updateCollisionBoxes() { _collidable->updateCollisionBoxes(this->state.current.pos); }
	virtual void testCollision(Collidable& collidable) { _collidable->testBoxCollision(collidable); }
};