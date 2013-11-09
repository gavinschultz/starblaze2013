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
#include "Phys.h"
#include <math.h>

bool handleEvent(SDL_Event*);
void handleState();
void integrate(double delta_time, double dt);
void integrateAlpha(double alpha);

std::unique_ptr<Game> game;
std::unique_ptr<Timer> timer;
std::unique_ptr<Renderer> renderer;
std::unique_ptr<World> world;

int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	game = std::unique_ptr<Game>{new Game()};
	timer = std::unique_ptr<Timer>{new Timer()};
	renderer = std::unique_ptr<Renderer>{new Renderer(1024, 768, 4.0)};
	world = std::unique_ptr<World>{new World(500)};
	Camera camera = Camera(SDL_Rect{ 0, 0, renderer->window.w, renderer->window.h }, SDL_Rect{ 48 * renderer->scaling, 0, 160 * renderer->scaling, 144 * renderer->scaling });
	camera.view_rect.x = -camera.focus_rect.x;
	camera.focus_rect.x = 0;
	game->max_ship_motion = { 1000.0, 800.0 };
	game->min_ship_motion_threshold_vel = { 30.0, 30.0 };
	game->min_ship_motion_threshold_secs = 0.6;
	game->mouse_sensitivity = 3.5;
	game->ship_limits = { 48 * renderer->scaling, 0 * renderer->scaling, 160 * renderer->scaling, 144 * renderer->scaling };

	// Load assets
	BGSprite* bg_sprite = new BGSprite(renderer.get(), world.get());
	renderer->sprite_register.registerSprite(bg_sprite);

	Ship* s = new Ship();
	s->direction = ShipDirection::right;
	s->bounding_box = { 0, 0, 32 * renderer->scaling, 8 * renderer->scaling };	// TODO: scaling real-time
	game->entity_register.registerEntity(s);
	ShipSprite* shipSprite = new ShipSprite(renderer.get(), s);
	renderer->sprite_register.registerSprite(shipSprite);

	renderer->sprite_register.registerSprite(new RadarSprite(renderer.get()));

	// end load assets

	game->start();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	Ship* ship = game->entity_register.getShip();
	ship->current_state.pos.x = 0.0;// game->ship_limits.x;
	ship->current_state.pos.y = game->ship_limits.y - ship->bounding_box.h;
	ship->current_state.vel.x = 0.0;
	ship->current_state.vel.y = 0.0;

	bool quit = false;
	double accumulator = 0;
	const double dt = 60.0 / 1000.0;
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

		/*ship->current_state.vel.x += ship->current_state.thrust.x;
		ship->current_state.vel.y += ship->current_state.thrust.y;
		ship->current_state.acc.x = (ship->current_state.vel.x - ship->prev_state.vel.x) * delta_time;
		ship->current_state.acc.y = (ship->current_state.vel.y - ship->prev_state.vel.y) * delta_time;*/

		accumulator += delta_time;
		while (accumulator >= dt)
		{
			ship->prev_state = ship->current_state;
			accumulator -= dt;
			integrate(delta_time, dt);
		}

		const double alpha = accumulator / dt;
		integrateAlpha(alpha);

		camera.focus_point = { std::lround(ship->alpha_pos.x), std::lround(ship->alpha_pos.y) };
		camera.focus_point_vel.x = std::lround(ship->alpha_pos.x - ship->prev_alpha_pos.x);
		//camera.view_rect.x = (int32_t)ship->alpha_state.pos.x - game->ship_limits.x;
		//camera.focus_rect.x = camera.view_rect.x + 48 * renderer->scaling;
		/*if (ship->direction == ShipDirection::right)
			camera.view_rect.x = ship->alpha_state.pos.x + game->ship_limits.x;
			else
			camera.view_rect.x = ship->alpha_state.pos.x + game->ship_limits.x + game->ship_limits.w;*/
		//int32_t camera_vel_x = camera.view_rect.x - camera.prev_view_rect.x;

		//debug({ "camera view/focus_rect/focus_point x:", std::to_string(camera.view_rect.x), "/", std::to_string(camera.focus_rect.x), "/", std::to_string(camera.focus_point.x), " vel x:", std::to_string(camera.focus_point_vel.x) });

		int32_t turn_speed = (int32_t)std::max(std::abs(lround(camera.focus_point_vel.x * 0.2)), 5L);
		//turn_speed = (int32_t)std::max(std::abs(std::lround(camera.focus_point_vel.x * 1.0)), 2L);
		if (ship->direction == ShipDirection::right)
		{
			camera.focus_rect.x = std::min(camera.focus_rect.x + camera.focus_point_vel.x + turn_speed, camera.focus_point.x);
		}
		else
		{
			camera.focus_rect.x = std::max(camera.focus_rect.x + camera.focus_point_vel.x - turn_speed, camera.focus_point.x + ship->bounding_box.w - camera.focus_rect.w);
		}
		camera.view_rect.x = camera.focus_rect.x - (48 * renderer->scaling);

		renderer->render(&camera);

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

	double reverse_thrust_multiplier = 800.0;
	double forward_thrust_multiplier = 40.0;
	double thrust_multiplier;

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
		case SDLK_F6:
			game->mouse_sensitivity = std::max(2.0f, game->mouse_sensitivity - 0.5f);
			break;
		case SDLK_F7:
			game->mouse_sensitivity = std::min(6.0f, game->mouse_sensitivity + 0.5f);
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
		debug({ "Vel x:", std::to_string(ship->current_state.vel.x), " acc x:", std::to_string(ship->current_state.acc.x) });
		if ((ship->current_state.vel.x > 0.0 && ship->current_state.acc.x > 0.0) || (ship->current_state.vel.x < 0.0 && ship->current_state.acc.x < 0.0))
		{
			thrust_multiplier = forward_thrust_multiplier;
			//debug({ "Forward thrust multiplier" });
		}
		else
		{
			thrust_multiplier = reverse_thrust_multiplier;
			//debug({ "Reverse thrust multiplier" });
		}
		ship->current_state.thrust.x = mousemotion.xrel * game->mouse_sensitivity * thrust_multiplier;
		ship->current_state.thrust.y = mousemotion.yrel * game->mouse_sensitivity * 4.5;
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
		ship->current_state.thrust.y = -ship->max_thrust * 0.1;
	if (keystate[SDL_SCANCODE_DOWN])
		ship->current_state.thrust.y = +ship->max_thrust * 0.1;
	if (keystate[SDL_SCANCODE_LEFT])
		ship->current_state.thrust.x = -ship->max_thrust;
	if (keystate[SDL_SCANCODE_RIGHT])
		ship->current_state.thrust.x = +ship->max_thrust;
}

void integrate(double delta_time, double dt)
{
	Ship* ship = game->entity_register.getShip();

	//double accel = std::max(-1000.0, ship->current_state.thrust.x * 50.0);
	if (ship->current_state.thrust.x > ship->max_thrust)
		ship->current_state.thrust.x = ship->max_thrust;
	else if (ship->current_state.thrust.x < -ship->max_thrust)
		ship->current_state.thrust.x = -ship->max_thrust;
	//ship->current_state.thrust.x = 50.0;

	double accel = ship->current_state.thrust.x / ship->weight;
	double vel = ship->current_state.vel.x + accel * dt;
	// decceleration
	if (std::abs(ship->current_state.vel.x) > 200 || std::abs(ship->current_state.thrust.x) > 0)
		vel -= ship->current_state.vel.x * (0.4 * dt);
	else
		vel -= ship->current_state.vel.x * (1.3 * dt);

	//if (ship->current_state.thrust.y == 0.0)
	ship->current_state.vel.y -= ship->current_state.vel.y * (4.0 * dt);


	double dist = (ship->current_state.vel.x * dt) + (accel * 0.5 * dt * dt);
	//debug({ "Thrust:", std::to_string(ship->current_state.thrust.x), " proposed vel x:", std::to_string(vel), " dist x:", std::to_string(dist), " accel x:", std::to_string(accel), "m/s/s current: ", std::to_string(ship->current_state.vel.x) });

	ship->current_state.acc.x = accel;
	ship->current_state.vel.x = vel;
	ship->current_state.vel.y += ship->current_state.thrust.y;
	ship->current_state.pos.x += dist;
	//ship->current_state.pos.x += ship->current_state.vel.x * dt;
	ship->current_state.pos.y += ship->current_state.vel.y * dt;

	/*if (ship->current_state.vel.x > game->max_ship_motion.x) ship->current_state.vel.x = game->max_ship_motion.x;
	if (ship->current_state.vel.x < -game->max_ship_motion.x) ship->current_state.vel.x = -game->max_ship_motion.x;*/
	if (ship->current_state.vel.y > game->max_ship_motion.y) ship->current_state.vel.y = game->max_ship_motion.y;
	if (ship->current_state.vel.y < -game->max_ship_motion.y) ship->current_state.vel.y = -game->max_ship_motion.y;

	ship->altitude = std::max(0.0, game->ship_limits.y + game->ship_limits.h - ship->current_state.pos.y - ship->bounding_box.h);
	//debug({ "Altitude:", std::to_string(ship->altitude) });

	/*if (ship->current_state.pos.x < game->ship_limits.x)
		ship->current_state.pos.x = game->ship_limits.x;
		else if (ship->current_state.pos.x + ship->bounding_box.w > game->ship_limits.x + game->ship_limits.w)
		ship->current_state.pos.x = game->ship_limits.x + game->ship_limits.w - ship->bounding_box.w;*/

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

	/*if (std::abs(ship->current_state.vel.x) < game->min_ship_motion_threshold_vel.x)
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
	}*/
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

	debug({ "ship x:", std::to_string(ship->current_state.pos.x), " world w:", std::to_string(world->w) });
	if (ship->current_state.pos.x > world->w)
	{
		ship->current_state.pos.x -= world->w;
	}

	//debug({ "Current x/limit:", std::to_string(ship->current_state.pos.x), "/", std::to_string(game->ship_limits.x), "/", std::to_string(game->ship_limits.x + game->ship_limits.w), " Current y:", std::to_string(ship->current_state.pos.y) });
	//debug({ "Current vel X:", std::to_string(ship->current_state.vel.x), " prev vel X: ", std::to_string(ship->prev_state.vel.x), " Y:", std::to_string(ship->current_state.vel.y), " secs at X slow: ", std::to_string(ship->secs_at_slow_vel_x), "secs at Y slow: ", std::to_string(ship->secs_at_slow_vel_y) });
	if (ship->current_state.vel.x > 0.0 || (ship->current_state.vel.x == 0.0 && ship->direction == ShipDirection::right))
		ship->direction = ShipDirection::right;
	else
		ship->direction = ShipDirection::left;
}

void integrateAlpha(double alpha)
{
	Ship* ship = game->entity_register.getShip();
	ship->prev_alpha_pos = ship->alpha_pos;
	ship->alpha_pos.x = ship->current_state.pos.x*alpha + ship->prev_state.pos.x*(1.0 - alpha);
	ship->alpha_pos.y = ship->current_state.pos.y*alpha + ship->prev_state.pos.y*(1.0 - alpha);
}