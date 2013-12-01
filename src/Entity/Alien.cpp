#include "stdafx.h"
#include "Alien.h"
#include "Phys.h"
#include "Entity\World.h"
#include <memory>
#include <functional>
#include <random>
#include <ctime>

class Alien::impl
{
public:
	static int rndseed;
	std::unique_ptr<AttackAlgorithm> attack_algorithm;
	static const std::vector<Rect> Alien::impl::base_collision_boxes;
};
int Alien::impl::rndseed{ 0 };
const std::vector<Rect> Alien::impl::base_collision_boxes{
	{ 6, 0, 4, 9 },
	{ 2, 2, 12, 3 },
	{ 4, 1, 8, 5 },
	{ 0, 3, 16, 1 },
	{ 0, 7, 16, 2 },
	{ 0, 9, 6, 3 },
	{ 10, 9, 6, 3 }
};

Alien::Alien() : pimpl{ new impl{} }
{
	bounding_box = { 0, 0, 16, 12 };
	collision_boxes = std::vector<Rect>{ *getBaseCollisionBoxes() };
	SDL_Rect play_area = world->getPlayArea(bounding_box);
	auto rnd_xpos = std::bind(std::uniform_real_distribution<double>{(double)play_area.x, (double)(play_area.x + play_area.w)}, std::default_random_engine((uint32_t)(std::time(0) + pimpl->rndseed++)));
	auto rnd_ypos = std::bind(std::uniform_real_distribution<double>{(double)play_area.y, (double)(play_area.y + play_area.h)}, std::default_random_engine((uint32_t)(std::time(0) + pimpl->rndseed++)));
	this->current_state.pos.x = rnd_xpos();
	this->current_state.pos.y = rnd_ypos();
	weight = 1.0;
	max_lift = 150.0;
	//pimpl->attack_algorithm = std::unique_ptr<AttackAlgorithm>{new DirectAttack{}};
	pimpl->attack_algorithm = std::unique_ptr<AttackAlgorithm>{new WaveAttack{}};
}
Alien::~Alien(){}

void Alien::runAI(const Entity* target, double dt)
{
	pimpl->attack_algorithm->run(*this, target, dt);
	
	//double angle_a_to_t = std::acos(dot_product);
	//debug->set("angle_a_to_t", angle_a_to_t);

	// Algorithm: thrust directly towards target

}

double Alien::getDecelerationFactorX() const
{
	//if (std::abs(current_state.vel.x) > 200 || std::abs(current_state.thrust.x) > 0)
		return 0.4;
}

const std::vector<Rect>* Alien::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}