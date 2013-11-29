#include "stdafx.h"
#include "Alien.h"
#include "Phys.h"
#include <memory>

Alien::Alien()
{
	bounding_box = { 0, 0, 16, 12 };
	weight = 1.0;
	max_lift = 60.0;
	//attack_algorithm = std::unique_ptr<AttackAlgorithm>{new DirectAttack{}};
	attack_algorithm = std::unique_ptr<AttackAlgorithm>{new WaveAttack{}};
}

void Alien::runAI(const Entity* target, double dt)
{
	attack_algorithm->run(*this, target, dt);
	
	//double angle_a_to_t = std::acos(dot_product);
	//debug->set("angle_a_to_t", angle_a_to_t);

	// Algorithm: thrust directly towards target

}

double Alien::getDecelerationFactorX() const
{
	//if (std::abs(current_state.vel.x) > 200 || std::abs(current_state.thrust.x) > 0)
		return 0.4;
}