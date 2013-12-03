#pragma once
#include <memory>

class Alien;
class Entity;

class AttackAlgorithm
{
public:
	virtual void run(Alien& alien, const Entity* target, double dt) = 0;
};

class DirectAttack : public AttackAlgorithm
{
private:
	class impl; std::unique_ptr<impl> pimpl;
public:
	DirectAttack();
	~DirectAttack();
	void run(Alien& alien, const Entity* target, double dt);
};

class WaveAttack : public AttackAlgorithm
{
private:
	class impl; std::unique_ptr<impl> pimpl;
public:
	WaveAttack();
	~WaveAttack();
	void run(Alien& alien, const Entity* target, double dt);
};

class DoNothingAttack : public AttackAlgorithm
{
public:
	void run(Alien& alien, const Entity* target, double dt);
};