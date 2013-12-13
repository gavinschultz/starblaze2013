#include "stdafx.h"
#include <array>
#include <random>
#include <functional>
#include <ctime>
#include <algorithm>
#include "playfield.h"
#include "phys.h"

PlayField::PlayField(const Window& window, int hill_count)
: boundaries{ 228.0, 0.0, window.w - 228.0, window.h - 192.0 }	// 1366 x 768 - wide-ish margins
{
	auto rnd_hill_y_channel = std::bind(std::uniform_int_distribution<int>{0, 3}, std::default_random_engine((uint32_t)time(0)));
	auto rnd_hill_x_channel = std::bind(std::uniform_int_distribution<int>{0, (int)(this->w / 8) - 1}, std::default_random_engine((uint32_t)time(0) - 1));
	auto rnd_type = std::bind(std::uniform_int_distribution<int>{0, 3}, std::default_random_engine((uint32_t)time(0) - 2));
	const std::array<double, 4> y_channel_speeds = { 0.1, 0.25, 0.5, 0.75 };
	hills.reserve(hill_count);
	for (int i = 0; i < hill_count; i++)
	{
		Hill hill = Hill();
		hill.y_channel = rnd_hill_y_channel();

		int x_channel = rnd_hill_x_channel();
		hill.x = x_channel * 8;

		if (hill.x > this->w * y_channel_speeds[hill.y_channel])
			hill.x = hill.x * y_channel_speeds[hill.y_channel];

		hill.distance_factor = y_channel_speeds[hill.y_channel];
		hill.type = rnd_type();
		hills.push_back(hill);
	}
}

Rect PlayField::getPlayArea(const Rect& entity_box)
{
	return{
		0.0,
		this->boundaries.y,
		this->w,
		this->boundaries.h - entity_box.h
	};
}

double PlayField::getAltitude(int entity_pos_y, int entity_box_h)
{
	return std::max(0.0, this->boundaries.y + this->boundaries.h - entity_pos_y - entity_box_h);
}