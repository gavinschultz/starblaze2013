#include "stdafx.h"
#include "physics.h"

#include <functional>
#include <random>
#include <ctime>

void PhysicsSystem::update(double dt)
{
	//auto rnd_sleep = std::bind(std::uniform_int_distribution<int>{0, 10}, std::default_random_engine((uint32_t)time(0)));
	//SDL_Delay(rnd_sleep());
}

void PhysicsSystem::interpolate(double alpha)
{

}