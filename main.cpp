#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include "timer.h"
#include <SDL_image.h>
#include <vector>
#include "Entity.h"
#include "Game.h"
#include <algorithm>
#include "Debug.h"
#include "Renderer.h"
#include <limits>

bool handleEvent(SDL_Event*);

std::unique_ptr<Game> game;
std::unique_ptr<Timer> timer;

int main(int argc, char* args[])
{
	game = std::unique_ptr<Game>{new Game()};
	timer = std::unique_ptr<Timer>{new Timer()};
	/*game.reset(new Game());
	timer.reset(new Timer());*/
	SDL_Init(SDL_INIT_EVERYTHING);

	//Renderer* renderer = new Renderer(2048, 1536, 0.5);
	//Renderer* renderer = new Renderer();
	Renderer* renderer = new Renderer(2048, 1536, 1.0);

	game->max_ship_motion = Vector2D(1000.0, 800.0);
	game->min_ship_motion_threshold_vel.x = 30.0;
	game->min_ship_motion_threshold_vel.y = 30.0;
	game->min_ship_motion_threshold_secs = 0.6;
	game->smooth_motion = true;
	game->ship_limits = { 192, 0, 640, 576 };
	
	// Load assets
	Ship* s = new Ship();
	game->entity_register.registerEntity(s);
	ShipSprite* shipSprite = new ShipSprite(renderer, s);
	renderer->sprite_register.registerSprite(shipSprite);

	game->start();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	Ship* ship = game->entity_register.getShip();
	ship->current_state.pos.x = game->ship_limits.x;
	ship->current_state.pos.y = game->ship_limits.y - ship->bounding_box.h;
	ship->current_state.vel.x = 0.0;
	ship->current_state.vel.y = 0.0;

	bool quit = false;
	double accumulator = 0;
	const double dt = 30.0 / 1000.0;
	SDL_Event pevent;

	while (!quit)
	{
		timer->startFrame();
		auto delta_time = timer->getLastFrameDuration();
		if (delta_time > 0.25)
		{
			OutputDebugString("Delta exceeded max");
			delta_time = 0.25;
		}

		ship->current_state.thrust.x = 0.0;
		ship->current_state.thrust.y = 0.0;

		while (SDL_PollEvent(&pevent))
		{
			if (!handleEvent(&pevent))
			{
				quit = true;
				break;
			}
		}
		if (quit)
			break;

		ship->current_state.acc.x = (ship->current_state.vel.x - ship->prev_state.vel.x);
		ship->current_state.acc.y = (ship->current_state.vel.y - ship->prev_state.vel.y);

		accumulator += delta_time;
		while (accumulator >= dt)
		{
			ship->prev_state = ship->current_state;
			accumulator -= dt;
			ship->current_state.pos.x -= ship->current_state.vel.x * dt;
			ship->current_state.pos.y += ship->current_state.vel.y * dt;

			if (ship->current_state.vel.x > game->max_ship_motion.x) ship->current_state.vel.x = game->max_ship_motion.x;
			if (ship->current_state.vel.x < -game->max_ship_motion.x) ship->current_state.vel.x = -game->max_ship_motion.x;
			if (ship->current_state.vel.y > game->max_ship_motion.y) ship->current_state.vel.y = game->max_ship_motion.y;
			if (ship->current_state.vel.y < -game->max_ship_motion.y) ship->current_state.vel.y = -game->max_ship_motion.y;

			if (abs(ship->current_state.vel.x) < game->min_ship_motion_threshold_vel.x)
			{
				ship->secs_at_slow_vel_x += delta_time;
				if (ship->secs_at_slow_vel_x >= game->min_ship_motion_threshold_secs)
				{
					ship->current_state.vel.x = 0.0;
					ship->secs_at_slow_vel_x = 0.0;
				}
			}
			else
			{
				ship->secs_at_slow_vel_x = 0.0;
			}
			if (abs(ship->current_state.vel.y) < game->min_ship_motion_threshold_vel.y)
			{
				ship->secs_at_slow_vel_y += delta_time;
				if (ship->secs_at_slow_vel_y >= game->min_ship_motion_threshold_secs)
				{
					ship->current_state.vel.y = 0.0;
					ship->secs_at_slow_vel_y = 0.0;
				}
			}
			else
			{
				ship->secs_at_slow_vel_y = 0.0;
			}

			if (ship->current_state.pos.x < game->ship_limits.x)
				ship->current_state.pos.x = game->ship_limits.x;
			else if (ship->current_state.pos.x + ship->bounding_box.w > game->ship_limits.x + game->ship_limits.w)
				ship->current_state.pos.x = game->ship_limits.x + game->ship_limits.w - ship->bounding_box.w;
			if (ship->current_state.pos.y < game->ship_limits.y)
				ship->current_state.pos.y = game->ship_limits.y;
			else if (ship->current_state.pos.y + ship->bounding_box.h > game->ship_limits.y + game->ship_limits.h)
				ship->current_state.pos.y = game->ship_limits.y + game->ship_limits.h - ship->bounding_box.h;

			//debug({ "Current x:", std::to_string(ship->current_state.pos.x), " Current y:", std::to_string(ship->current_state.pos.y) });
			//debug({ "Current vel X:", std::to_string(ship->current_state.vel.x), " prev vel X: ", std::to_string(ship->prev_state.vel.x), " Y:", std::to_string(ship->current_state.vel.y), " secs at X slow: ", std::to_string(ship->secs_at_slow_vel_x), "secs at Y slow: ", std::to_string(ship->secs_at_slow_vel_y) });
			if (ship->current_state.vel.x < 0.0 || (ship->current_state.vel.x == 0.0 && ship->direction == ShipDirection::left))
				ship->direction = ShipDirection::left;
			else
				ship->direction = ShipDirection::right;
		}

		const double alpha = accumulator / dt;

		ship->alpha_state.pos.x = ship->current_state.pos.x*alpha + ship->prev_state.pos.x*(1.0 - alpha);
		ship->alpha_state.pos.y = ship->current_state.pos.y*alpha + ship->prev_state.pos.y*(1.0 - alpha);

		renderer->render();

		timer->endFrame();
	}

	SDL_Quit();
	return 0;
}

bool handleEvent(SDL_Event* pevent)
{
	if (pevent == NULL)
		return true;

	Ship* ship = game->entity_register.getShip();

	SDL_KeyboardEvent key;
	SDL_MouseMotionEvent mousemotion;
	switch (pevent->type)
	{
	case SDL_KEYDOWN:
		key = pevent->key;
		switch (pevent->key.keysym.sym)
		{
		case SDLK_ESCAPE:
		case SDLK_q:
			return false;
			break;
		case SDLK_LEFT:
			ship->current_state.vel.x += ship->current_state.vel.x - 150.0;
			break;
		case SDLK_RIGHT:
			ship->current_state.vel.x += ship->current_state.vel.x + 150.0;
			break;
		case SDLK_UP:
			ship->current_state.vel.y += ship->current_state.vel.y - 75.0;
			break;
		case SDLK_DOWN:
			ship->current_state.vel.y += ship->current_state.vel.y + 75.0;
			break;
		case SDLK_F10:
			game->smooth_motion = !game->smooth_motion;
			break;
		}
		break;
	case SDL_MOUSEMOTION:
		mousemotion = pevent->motion;
		//debug({ "Mouse move Xrel:", std::to_string(mousemotion.xrel), " Yrel:", std::to_string(mousemotion.yrel) });
		ship->current_state.thrust.x = mousemotion.xrel * 3.0;
		ship->current_state.thrust.y = mousemotion.yrel * 3.0;
		ship->current_state.vel.x += ship->current_state.thrust.x;
		ship->current_state.vel.y += ship->current_state.thrust.y;
		//debug({ "Mouse xrel:", std::to_string(mousemotion.xrel), " yrel:", std::to_string(mousemotion.yrel), " Ship current vel X:", std::to_string(ship->current_state.vel.x), " vel Y:", std::to_string(ship->current_state.vel.y), " prev vel X:", std::to_string(ship->prev_state.vel.x), " vel Y:", std::to_string(ship->prev_state.vel.y) });
		break;
	default:
		break;
	}

	int x, y;
	SDL_GetMouseState(&x, &y);

	return true;
}