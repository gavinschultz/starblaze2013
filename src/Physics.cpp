#include "stdafx.h"
#include "physics.h"

#include <functional>
#include <random>
#include <ctime>

void PhysicsSystem::update()
{
	auto rnd_sleep = std::bind(std::uniform_int_distribution<int>{0, 100}, std::default_random_engine((uint32_t)time(0)));
	//SDL_Delay(rnd_sleep());
}