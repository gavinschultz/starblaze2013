#include "stdafx.h"
#include <memory>
#include <functional>
#include <random>
#include <ctime>
#include "Alien.h"
#include "Phys.h"
#include "World.h"
#include "Bullet.h"
#include "Ship.h"
#include "Debris.h"
#include "Game.h"
#include "Collidable.h"

class Alien::impl
{
public:
	static int rndseed;
	std::unique_ptr<AttackAlgorithm> attack_algorithm;
	static const std::vector<Rect> base_collision_boxes;
	static const Rect base_outer_box;
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
const Rect base_outer_box = { 0, 0, 16, 12 };

Alien::Alien() : Entity(new NormalCollidable{ impl::base_outer_box, impl::base_collision_boxes, { defaultShipToAlienCollider.get(), defaultShipToStationCollider.get() } }) pimpl{ new impl{} }
{
	this->box = { 0, 0, 16, 12 };
	this->_collidable = std::make_unique<Collidable>(new NormalCollidable(box, *getBaseCollisionBoxes(), { }));

	this->attrib.weight = 100.0;
	this->attrib.max_lift = 150.0;
	this->attrib.health = 100;

	this->deceleration_factor = { 0.4, 1.0 };

	Rect play_area = world->getPlayArea(box);
	auto rnd_xpos = std::bind(std::uniform_real_distribution<double>{(double)play_area.x, (double)(play_area.x + play_area.w)}, std::default_random_engine((uint32_t)(std::time(0) + pimpl->rndseed++)));
	auto rnd_ypos = std::bind(std::uniform_real_distribution<double>{(double)play_area.y, (double)(play_area.y + play_area.h)}, std::default_random_engine((uint32_t)(std::time(0) + pimpl->rndseed++)));
	this->state.current.pos.x = rnd_xpos();
	this->state.current.pos.y = rnd_ypos();
	
	//pimpl->attack_algorithm = std::unique_ptr<AttackAlgorithm>{new DirectAttack{}};
	pimpl->attack_algorithm = std::unique_ptr<AttackAlgorithm>{new WaveAttack{}};
	//pimpl->attack_algorithm = std::unique_ptr<AttackAlgorithm>{new DoNothingAttack{}};
}
Alien::~Alien(){}

void Alien::runAI(const Entity* target, double dt)
{
	pimpl->attack_algorithm->run(*this, target, dt);
}

void Alien::damage(int points)
{
	Entity::damage(points);
	if (attrib.health > 0)
		return;
	game->entity_register.registerEntity(new Debris{ this->state.current });
}

const std::vector<Rect>* Alien::getBaseCollisionBoxes() const
{
	return &(pimpl->base_collision_boxes);
}