#include "stdafx.h"
#include <random>
#include <functional>
#include <ctime>
#include "AttackAlgorithm.h"
#include "Entity\Entity.h"
#include "Entity\Alien.h"
#include "MathUtil.h"
#include "Debug.h"

class WaveAttack::impl
{
private:
	static int rndseed;
	std::function<double()> rnd_magnitude{ std::bind(std::uniform_real_distribution<double>{85.0, 115.0}, std::default_random_engine{ (uint32_t)(std::time(0) + (++rndseed)) }) };
	std::function<double()> rnd_thrust{ std::bind(std::uniform_real_distribution<double>{340.0, 355.0}, std::default_random_engine{ (uint32_t)(std::time(0) + (++rndseed)) }) };
	std::function<double()> rnd_multiplier{ std::bind(std::uniform_real_distribution<double>{0.8, 1.2}, std::default_random_engine{ (uint32_t)(std::time(0) + (++rndseed)) }) };
public:
	double hz = 1.0;	// cycles per second
	double counter = 0.0;
	double magnitude = rnd_magnitude();
	double thrust = rnd_thrust();
	double sin_multiplier = rnd_multiplier();
};

int WaveAttack::impl::rndseed{ 0 };

WaveAttack::WaveAttack() : pimpl{ new impl{} }{}
WaveAttack::~WaveAttack(){}
void WaveAttack::run(Alien& alien, const Entity* target, double dt)
{
	pimpl->counter += dt;
	double sin_y = std::sin(std::fmod(pimpl->counter, 6.28)) * pimpl->sin_multiplier;
	//debug->set("counter", pimpl->counter);
	//debug->set("(sin_y)", (sin_y));
	
	alien.current_state.thrust.x = pimpl->thrust;
	alien.current_state.vel.y = pimpl->magnitude * (sin_y);
}