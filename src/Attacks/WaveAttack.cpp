#include "stdafx.h"
#include <random>
#include <functional>
#include <ctime>
#include <algorithm>
#include "AttackAlgorithm.h"
#include "Entity\Entity.h"
#include "Entity\Alien.h"
#include "MathUtil.h"
#include "Util.h"
#include "Debug.h"
#include "Entity\World.h"

class WaveAttack::impl
{
private:
	static int rndseed;
	std::function<double()> rnd_magnitude{ std::bind(std::uniform_real_distribution<double>{85.0, 115.0}, std::default_random_engine{ (uint32_t)(std::time(0) + (++rndseed)) }) };
	std::function<double()> rnd_thrust{ std::bind(std::uniform_real_distribution<double>{180.0, 220.0}, std::default_random_engine{ (uint32_t)(std::time(0) + (++rndseed)) }) };
	std::function<double()> rnd_multiplier{ std::bind(std::uniform_real_distribution<double>{0.5, 0.9}, std::default_random_engine{ (uint32_t)(std::time(0) + (++rndseed)) }) };
	std::function<double()> rnd_initial_radians{ std::bind(std::uniform_real_distribution<double>{0.0, 6.28}, std::default_random_engine{ (uint32_t)(std::time(0) + (++rndseed)) }) };
public:
	double hz = 1.0;	// cycles per second
	double radians_counter = rnd_initial_radians();
	double magnitude = rnd_magnitude();
	double thrust = rnd_thrust();
	double sin_multiplier = rnd_multiplier();
};

int WaveAttack::impl::rndseed{ 0 };

WaveAttack::WaveAttack() : pimpl{ new impl{} }{}
WaveAttack::~WaveAttack(){}
void WaveAttack::run(Alien& alien, const Entity* target, double dt)
{
	Vector2D vec_a_to_t = { target->current_state.pos.x - alien.current_state.pos.x, target->current_state.pos.y - alien.current_state.pos.y };
	double magnitude = 0.0;
	Vector2D vec_a_to_t_norm = mathutil::normaliseVector(vec_a_to_t, &magnitude);

	double relative_x = util::relativeX(target->current_state.pos.x, alien.current_state.pos.x, world->w);
	pimpl->radians_counter += dt;
	double sin_y = std::sin(std::fmod(pimpl->radians_counter, 6.28)) * pimpl->sin_multiplier;
	
	double thrust_factor = 0.6 + (std::min(std::abs(relative_x), alien.max_thrust*4) / (alien.max_thrust * 4));
	//debug->set("relative_x", relative_x);
	//debug->set("magnitude", magnitude);
	//debug->set("thrust_factor", thrust_factor);
	alien.current_state.thrust.x = pimpl->thrust * thrust_factor;
	alien.current_state.vel.y = pimpl->magnitude * (sin_y);
}