#pragma once
#include <memory>
#include "Entity.h"
#include "Renderer.h"

class Game
{
public:
	Game();
	~Game();
	void start();
	EntityRegister entity_register;
	Vector2D max_ship_motion;
	Vector2D min_ship_motion_threshold_vel;
	double min_ship_motion_threshold_secs;
	SDL_Rect ship_limits;
	float mouse_sensitivity;
};

extern std::unique_ptr<Game> game;