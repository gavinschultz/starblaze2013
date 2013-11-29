#pragma once
#include <memory>
#include "Phys.h"
#include "Entity.h"
#include "Attacks\AttackAlgorithm.h"

class EntityRegister;

class Alien : public Entity
{
private:
	std::unique_ptr<AttackAlgorithm> attack_algorithm;
public:
	Alien();
	double altitude{ 0.0 };
	Vector2D attack_vector_n;

	// The AI may consist of:
	// a) selecting a new target
	// b) setting the thrust required to move towards that target
	// c) firing
	// The target may be null, in which case it's up to the alien to work out what to do while "twiddling its thumbs"
	void runAI(const Entity* target, double dt);
	double getDecelerationFactorX() const;
};