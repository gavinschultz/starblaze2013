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
	std::function<double()> rnd_multiplier{ std::bind(std::uniform_real_distribution<double>{0.5, 0.8}, std::default_random_engine{ (uint32_t)(std::time(0) + (++rndseed)) }) };
	std::function<double()> rnd_initial_radians{ std::bind(std::uniform_real_distribution<double>{0.0, 6.28}, std::default_random_engine{ (uint32_t)(std::time(0) + (++rndseed)) }) };
public:
	double hz = 1.5;	// cycles per second
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
	Vector2D vec_a_to_t = { target->state.current.pos.x - alien.state.current.pos.x, target->state.current.pos.y - alien.state.current.pos.y };
	double magnitude = 0.0;
	Vector2D vec_a_to_t_norm = mathutil::normaliseVector(vec_a_to_t, &magnitude);

	//double sin_y = std::sin(std::fmod(pimpl->radians_counter, 6.28)) * pimpl->sin_multiplier;

	double relative_x = util::relativeX(target->state.current.pos.x, alien.state.current.pos.x, world->w);
	double thrust_factor = 1.5;
	if (std::abs(relative_x) < 1024)
		//thrust_factor = relative_x / 1280 + 0.2;
		thrust_factor = (std::abs(relative_x) / 2560 + 0.1);
	thrust_factor *= std::copysign(1.0, vec_a_to_t.x);
	//double thrust_factor = 0.6 + (std::min(std::abs(relative_x), alien.max_thrust*4) / (alien.max_thrust * 4));
	alien.state.current.thrust.x = pimpl->thrust * thrust_factor;

	pimpl->radians_counter += dt; // *(1 / pimpl->sin_multiplier) *(1 / pimpl->hz);
	double midpoint_y = (144.0 - alien.box.h) / 2;
	double sin_y = midpoint_y * std::sin(pimpl->hz * (pimpl->radians_counter)) + midpoint_y;
	alien.state.current.pos.y = sin_y;
	debug->set("sin_y", sin_y);
}