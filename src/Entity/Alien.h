#pragma once
#include <memory>
#include "Phys.h"
#include "Entity.h"
#include "Attacks\AttackAlgorithm.h"

class EntityRegister;

class Alien : public Entity
{
private:
	class impl; std::unique_ptr<impl> pimpl;
public:
	Alien();
	~Alien();
	double altitude{ 0.0 };
	double max_thrust{ 380.0 };
	Vector2D attack_vector_n;

	// The AI may consist of:
	// a) selecting a new target
	// b) setting the thrust required to move towards that target
	// c) firing
	// The target may be null, in which case it's up to the alien to work out what to do while "twiddling its thumbs"
	void runAI(const Entity* target, double dt);
	double getDecelerationFactorX() const;
};