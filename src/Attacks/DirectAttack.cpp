#include "stdafx.h"
#include <random>
#include <functional>
#include <ctime>
#include "AttackAlgorithm.h"
#include "Entity\Entity.h"
#include "Entity\Alien.h"
#include "Entity\World.h"
#include "MathUtil.h"
#include "Util.h"
#include "Debug.h"

class DirectAttack::impl
{
private:
	static int rndseed;
	double reaction_time_sec{ 1.0 };	// time in seconds before being able to react to a thrust reversal
	std::function<double()> rnd_reaction_time{ std::bind(std::uniform_real_distribution<double>{0.04, 0.7}, std::default_random_engine{ (uint32_t)(std::time(0) + rndseed++) }) };
public:
	double thrust_changed_without_reaction_sec{ 0.0 };
	void resetReactionTime()
	{
		reaction_time_sec = rnd_reaction_time();
		thrust_changed_without_reaction_sec = reaction_time_sec;
		//debug->set("reaction_time_sec", reaction_time_sec);
	};
};
int DirectAttack::impl::rndseed{ 0 };

DirectAttack::DirectAttack() : pimpl{ new impl{} }{}
DirectAttack::~DirectAttack(){}
void DirectAttack::run(Alien& alien, const Entity* target, double dt)
{
	Vector2D vec_alien = { alien.state.current.pos.x + (alien.box.w / 2), alien.state.current.pos.y + (alien.box.h / 2) };
	Vector2D vec_target = { target->state.current.vel.x, target->state.current.vel.y };
	double rel_x = util::relativeX(alien.state.current.pos.x, target->state.current.pos.x, world->w);
	Vector2D vec_a_to_t = { rel_x, target->state.current.pos.y - alien.state.current.pos.y };

	double magnitude = 0.0;
	Vector2D vec_a_to_t_norm = mathutil::normaliseVector(vec_a_to_t, &magnitude);
	debug->set("a_to_t magnitude", magnitude);
	debug->set("rel_x", rel_x);
	//debug->set("vec_a_to_t.x", vec_a_to_t.x);
	//debug->set("vec_a_to_t.y", vec_a_to_t.y);

	alien.attack_vector_n = vec_a_to_t_norm;

	// calculate normal
	/*Vector2D vec_alien_norm = mathutil::normaliseVector(vec_alien, &magnitude);
	double dot_product = vec_a_to_t_norm.x * vec_alien_norm.x + vec_a_to_t_norm.y * vec_alien_norm.y;
	debug->set("dot_product", dot_product);*/

	if ((target->state.current.vel.y != 0.0 || std::copysign(1.0, alien.state.current.thrust.y) != std::copysign(1.0, alien.state.prev.thrust.y))
		&& (std::copysign(1.0, alien.state.current.thrust.y) != std::copysign(1.0, target->state.current.thrust.y)))
	{
		if (pimpl->thrust_changed_without_reaction_sec <= 0.0)
			pimpl->resetReactionTime();
		pimpl->thrust_changed_without_reaction_sec -= dt;
	}
	else
	{
		pimpl->thrust_changed_without_reaction_sec = 0.0;
	}

	if (pimpl->thrust_changed_without_reaction_sec <= 0.0)
	{
		//alien.state.current.thrust.x = attack_vector_n.x * 20.0;
		alien.state.current.thrust.x = std::copysign(1.0, alien.attack_vector_n.x) * 325.0; //std::abs(target->state.current.thrust.x);
		alien.state.current.thrust.y = alien.attack_vector_n.y * 25.0;
	}

	double relative_vel = target->state.current.vel.x - alien.state.current.vel.x;
	debug->set("relative_vel", relative_vel);
	if (std::abs(relative_vel) > 100 && std::abs(magnitude) > 400 && std::abs(magnitude) < 800)
		alien.state.current.thrust.x *= 0.5;
	if (std::abs(relative_vel) > 100 && std::abs(magnitude) < 400)
		alien.state.current.thrust.x *= 0.25;
	debug->set("alien.state.current.thrust.x", alien.state.current.thrust.x);

	//debug->set("target.thrust.x", target->state.current.thrust.x);
	//debug->set("alien.thrust.x", alien.state.current.thrust.x);
	//debug->set("target.vel.x", target->state.current.vel.x);
	//debug->set("alien.vel.x", alien.state.current.vel.x);
	//debug->set("pimpl->thrust_changed_without_reaction_sec", pimpl->thrust_changed_without_reaction_sec);
}