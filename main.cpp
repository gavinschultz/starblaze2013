#pragma comment(lib, "glew32.lib")
#include <SDL.h>
//#include <Windows.h>
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

bool handleEvent(SDL_Event*);
void handleState();
void integrate(double delta_time, double dt);
void integrateAlpha(double alpha);

std::unique_ptr<Game> game;
std::unique_ptr<Timer> timer;
std::unique_ptr<Renderer> renderer;

int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	game = std::unique_ptr<Game>{new Game()};
	timer = std::unique_ptr<Timer>{new Timer()};
	renderer = std::unique_ptr<Renderer>{new Renderer(1024, 768, 4.0)};

	game->max_ship_motion = { 1000.0, 800.0 };
	game->min_ship_motion_threshold_vel = { 30.0, 30.0 };
	game->min_ship_motion_threshold_secs = 0.6;
	game->smooth_motion = true;
	game->mouse_sensitivity = 3.5;
	game->ship_limits = { 48 * renderer->scaling, 0 * renderer->scaling, 160 * renderer->scaling, 144 * renderer->scaling };

	// Load assets
	World world{ 100 };	// auto world = std::unique_ptr<World>{new World()};
	BGSprite* bg_sprite = new BGSprite(renderer.get(), &world);
	renderer->sprite_register.registerSprite(bg_sprite);

	Ship* s = new Ship();
	s->bounding_box = { 0, 0, 32 * renderer->scaling, 8 * renderer->scaling };	// TODO: scaling real-time
	game->entity_register.registerEntity(s);
	ShipSprite* shipSprite = new ShipSprite(renderer.get(), s);
	renderer->sprite_register.registerSprite(shipSprite);

	renderer->sprite_register.registerSprite(new RadarSprite(renderer.get()));

	// end load assets

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
			debug({ "Delta exceeded max" });
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
		handleState();
		if (quit)
			break;

		ship->current_state.vel.x += ship->current_state.thrust.x;
		ship->current_state.vel.y += ship->current_state.thrust.y;
		ship->current_state.acc.x = (ship->current_state.vel.x - ship->prev_state.vel.x);
		ship->current_state.acc.y = (ship->current_state.vel.y - ship->prev_state.vel.y);

		accumulator += delta_time;
		while (accumulator >= dt)
		{
			ship->prev_state = ship->current_state;
			accumulator -= dt;
			integrate(delta_time, dt);
		}

		const double alpha = accumulator / dt;
		integrateAlpha(alpha);

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
		case SDLK_F10:
			game->smooth_motion = !game->smooth_motion;
			break;
		case SDLK_F6:
			game->mouse_sensitivity = std::max(2.0, game->mouse_sensitivity - 0.5);
			break;
		case SDLK_F7:
			game->mouse_sensitivity = std::min(6.0, game->mouse_sensitivity + 0.5);
			break;
		case SDLK_F11:
			renderer->toggleGrid(!renderer->is_grid_visible);
			break;
		case SDLK_RETURN:
			if (pevent->key.keysym.mod & KMOD_ALT)
				renderer->toggleFullscreen(!renderer->is_fullscreen);
			break;
		}
		break;
	case SDL_MOUSEMOTION:
		mousemotion = pevent->motion;
		//debug({ "Mouse move Xrel:", std::to_string(mousemotion.xrel), " Yrel:", std::to_string(mousemotion.yrel) });
		ship->current_state.thrust.x = mousemotion.xrel * game->mouse_sensitivity;
		ship->current_state.thrust.y = mousemotion.yrel * game->mouse_sensitivity;
		//debug({ "Mouse xrel:", std::to_string(mousemotion.xrel), " yrel:", std::to_string(mousemotion.yrel), " Ship current vel X:", std::to_string(ship->current_state.vel.x), " vel Y:", std::to_string(ship->current_state.vel.y), " prev vel X:", std::to_string(ship->prev_state.vel.x), " vel Y:", std::to_string(ship->prev_state.vel.y) });
		break;
	default:
		break;
	}

	int x, y;
	SDL_GetMouseState(&x, &y);

	return true;
}

void handleState()
{
	auto* keystate = SDL_GetKeyboardState(NULL);
	Ship* ship = game->entity_register.getShip();
	if (keystate[SDL_SCANCODE_UP])
		ship->current_state.thrust.y = -75.0;
	if (keystate[SDL_SCANCODE_DOWN])
		ship->current_state.thrust.y = +75.0;
	if (keystate[SDL_SCANCODE_LEFT])
		ship->current_state.thrust.x = -75.0;
	if (keystate[SDL_SCANCODE_RIGHT])
		ship->current_state.thrust.x = +75.0;
}

void integrate(double delta_time, double dt)
{
	Ship* ship = game->entity_register.getShip();
	ship->current_state.pos.x -= ship->current_state.vel.x * dt;
	ship->current_state.pos.y += ship->current_state.vel.y * dt;

	if (ship->current_state.vel.x > game->max_ship_motion.x) ship->current_state.vel.x = game->max_ship_motion.x;
	if (ship->current_state.vel.x < -game->max_ship_motion.x) ship->current_state.vel.x = -game->max_ship_motion.x;
	if (ship->current_state.vel.y > game->max_ship_motion.y) ship->current_state.vel.y = game->max_ship_motion.y;
	if (ship->current_state.vel.y < -game->max_ship_motion.y) ship->current_state.vel.y = -game->max_ship_motion.y;

	ship->altitude = std::max(0.0, game->ship_limits.y + game->ship_limits.h - ship->current_state.pos.y - ship->bounding_box.h);
	//debug({ "Altitude:", std::to_string(ship->altitude) });
	if (ship->current_state.pos.x < game->ship_limits.x)
		ship->current_state.pos.x = game->ship_limits.x;
	else if (ship->current_state.pos.x + ship->bounding_box.w > game->ship_limits.x + game->ship_limits.w)
		ship->current_state.pos.x = game->ship_limits.x + game->ship_limits.w - ship->bounding_box.w;
	if (ship->current_state.pos.y < game->ship_limits.y)
	{
		ship->current_state.pos.y = game->ship_limits.y;
		ship->current_state.vel.y = 0.0;
	}
	else if (ship->altitude == 0.0)
	{
		ship->current_state.pos.y = game->ship_limits.y + game->ship_limits.h - ship->bounding_box.h;
		ship->current_state.vel.y = 0.0;
	}

	if (std::abs(ship->current_state.vel.x) < game->min_ship_motion_threshold_vel.x)
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
	if (std::abs(ship->current_state.vel.y) < game->min_ship_motion_threshold_vel.y)
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

	//debug({ "Current x/limit:", std::to_string(ship->current_state.pos.x), "/", std::to_string(game->ship_limits.x), "/", std::to_string(game->ship_limits.x + game->ship_limits.w), " Current y:", std::to_string(ship->current_state.pos.y) });
	//debug({ "Current vel X:", std::to_string(ship->current_state.vel.x), " prev vel X: ", std::to_string(ship->prev_state.vel.x), " Y:", std::to_string(ship->current_state.vel.y), " secs at X slow: ", std::to_string(ship->secs_at_slow_vel_x), "secs at Y slow: ", std::to_string(ship->secs_at_slow_vel_y) });
	if (ship->current_state.vel.x < 0.0 || (ship->current_state.vel.x == 0.0 && ship->direction == ShipDirection::left))
		ship->direction = ShipDirection::left;
	else
		ship->direction = ShipDirection::right;
}

void integrateAlpha(double alpha)
{
	Ship* ship = game->entity_register.getShip();
	ship->alpha_state.pos.x = ship->current_state.pos.x*alpha + ship->prev_state.pos.x*(1.0 - alpha);
	ship->alpha_state.pos.y = ship->current_state.pos.y*alpha + ship->prev_state.pos.y*(1.0 - alpha);
}