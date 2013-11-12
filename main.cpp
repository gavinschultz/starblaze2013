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
std::unique_ptr<Debug> debug;

int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	game = std::unique_ptr<Game>{new Game()};
	timer = std::unique_ptr<Timer>{new Timer()};
	renderer = std::unique_ptr<Renderer>{new Renderer(1024, 768, 4.0)};
	debug = std::unique_ptr<Debug>{new Debug()};
#ifdef _DEBUG
	renderer->toggleGrid(true);
#endif
	world = std::unique_ptr<World>{new World(25)};
	Camera camera = Camera(SDL_Rect{ 0, 0, renderer->window.w, renderer->window.h }, SDL_Rect{ 48 * renderer->scaling, 0, 160 * renderer->scaling, 144 * renderer->scaling });
	//camera.focus_rect.x = +192;
	game->ship_limits = { 48 * renderer->scaling, 0 * renderer->scaling, 160 * renderer->scaling, 144 };

	// Load assets
	BGSprite* bg_sprite = new BGSprite(renderer.get(), world.get());
	renderer->sprite_register.registerSprite(bg_sprite);

	Ship* s = new Ship();
	s->direction = ShipDirection::right;
	s->bounding_box = { 0, 0, 32, 8 };	// TODO: scaling real-time
	game->entity_register.registerEntity(s);
	ShipSprite* shipSprite = new ShipSprite(renderer.get(), s);
	renderer->sprite_register.registerSprite(shipSprite);

	renderer->sprite_register.registerSprite(new RadarSprite(renderer.get()));

	// end load assets

	game->start();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	Ship* ship = game->entity_register.getShip();
	ship->current_state.pos.x = 45.5;// game->ship_limits.x;
	ship->current_state.pos.y = game->ship_limits.y - ship->bounding_box.h;
	ship->current_state.vel.x = 0.0;
	ship->current_state.vel.y = 0.0;

	bool quit = false;
	double accumulator = 0;
	const double dt = 1.0 / 60.0;
	SDL_Event pevent;

	while (!quit)
	{
		timer->startFrame();
		auto delta_time = timer->getLastFrameDuration();
		if (delta_time > 0.25)
		{
			console_debug({ "Delta exceeded max" });
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

		accumulator += delta_time;
		while (accumulator >= dt)
		{
			ship->prev_state = ship->current_state;
			accumulator -= dt;
			integrate(delta_time, dt);
		}

		const double alpha = accumulator / dt;
		integrateAlpha(alpha);

		camera.prev_focus_point = camera.focus_point;
		camera.prev_focus_loop_count = camera.focus_loop_count;
		camera.prev_view_rect = camera.view_rect;
		camera.focus_point = { std::lround(ship->alpha_pos.x * renderer->scaling), std::lround(ship->alpha_pos.y * renderer->scaling) };
		camera.focus_loop_count = ship->current_state.loop_count;

		//camera.focus_point_vel.x = camera.focus_point.x - camera.prev_focus_point.x;
		camera.focus_point_vel.x = camera.focus_point.x - camera.prev_focus_point.x + ((camera.focus_loop_count - camera.prev_focus_loop_count) * world->w * renderer->scaling);

		if (camera.focus_loop_count != camera.prev_focus_loop_count)
		{
			console_debug({ "Focus point a/b: ", std::to_string(camera.focus_point.x), "/", std::to_string(camera.prev_focus_point.x), " focus point vel X:", std::to_string(camera.focus_point_vel.x) });
		}

		debug->set("cam view X", camera.view_rect.x);
		debug->set("cam foc_rct X", camera.focus_rect.x);
		debug->set("cam foc_pt X", camera.focus_point.x);
		debug->set("cam vel X", camera.focus_point_vel.x);
		debug->set("ship alpha X", ship->alpha_pos.x);
		//debug->set("ship current X", ship->current_state.pos.x);
		debug->set("loop count", ship->current_state.loop_count);

		int32_t turn_speed = (int32_t)std::max(std::abs(lround(camera.focus_point_vel.x * 0.2)), 5L);
		if (ship->direction == ShipDirection::right)
		{
			camera.focus_rect.x = std::min(camera.focus_rect.x + camera.focus_point_vel.x + turn_speed, camera.focus_point.x);
		}
		else
		{
			camera.focus_rect.x = std::max(camera.focus_rect.x + camera.focus_point_vel.x - turn_speed, camera.focus_point.x + (ship->bounding_box.w * (int)renderer->scaling) - camera.focus_rect.w);
		}
		camera.view_rect.x = camera.focus_rect.x - (48 * renderer->scaling);
		if (camera.view_rect.x < 0)
			camera.view_rect.x += world->w * renderer->scaling;

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
		//debug({ "Vel x:", std::to_string(ship->current_state.vel.x), " acc x:", std::to_string(ship->current_state.acc.x) });
		if ((ship->current_state.vel.x > 0.0 && ship->current_state.acc.x > 0.0) || (ship->current_state.vel.x < 0.0 && ship->current_state.acc.x < 0.0))
		{
			thrust_multiplier = forward_thrust_multiplier;;
		}
		else
		{
			thrust_multiplier = reverse_thrust_multiplier;
		}
		ship->current_state.thrust.x = mousemotion.xrel * game->mouse_sensitivity * thrust_multiplier;
		ship->current_state.thrust.y = mousemotion.yrel * game->mouse_sensitivity * 0.8;
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
		ship->current_state.thrust.y = -ship->max_thrust * 0.04;
	if (keystate[SDL_SCANCODE_DOWN])
		ship->current_state.thrust.y = +ship->max_thrust * 0.04;
	if (keystate[SDL_SCANCODE_LEFT])
		ship->current_state.thrust.x = -ship->max_thrust;
	if (keystate[SDL_SCANCODE_RIGHT])
		ship->current_state.thrust.x = +ship->max_thrust;
}

void integrate(double delta_time, double dt)
{
	Ship* ship = game->entity_register.getShip();

	if (ship->current_state.thrust.x > ship->max_thrust)
		ship->current_state.thrust.x = ship->max_thrust;
	else if (ship->current_state.thrust.x < -ship->max_thrust)
		ship->current_state.thrust.x = -ship->max_thrust;

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
	ship->current_state.pos.y += ship->current_state.vel.y * dt;

	if (ship->current_state.vel.y > ship->max_lift) ship->current_state.vel.y = ship->max_lift;
	if (ship->current_state.vel.y < -ship->max_lift) ship->current_state.vel.y = -ship->max_lift;

	ship->altitude = std::max(0.0, game->ship_limits.y + game->ship_limits.h - ship->current_state.pos.y - ship->bounding_box.h);
	//debug({ "Altitude:", std::to_string(ship->altitude) });

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

	//debug({ "ship x:", std::to_string(ship->current_state.pos.x), " world w:", std::to_string(world->w) });
	if (ship->current_state.pos.x > world->w)
	{
		ship->current_state.pos.x -= world->w;
		ship->current_state.loop_count++;
	}
	else if (ship->current_state.pos.x < 0.0)
	{
		ship->current_state.pos.x += world->w;
		ship->current_state.loop_count--;
	}

	if (ship->current_state.vel.x > 0.0 || (ship->current_state.vel.x == 0.0 && ship->direction == ShipDirection::right))
		ship->direction = ShipDirection::right;
	else
		ship->direction = ShipDirection::left;

	//debug({ "integrate ship.current.x: ", std::to_string(ship->current_state.pos.x) });
}

void integrateAlpha(double alpha)
{
	Ship* ship = game->entity_register.getShip();
	//ship->prev_alpha_pos = ship->alpha_pos;

	if (ship->current_state.loop_count != ship->prev_state.loop_count)
	{
		double wrap_factor = ((ship->current_state.loop_count - ship->prev_state.loop_count)*world->w);
		double currentX = ship->current_state.pos.x*alpha;
		double prevX = (ship->prev_state.pos.x - wrap_factor)*(1.0 - alpha);
		ship->alpha_pos.x = currentX + prevX;
		//console_debug({ "current/prev X: ", std::to_string(ship->current_state.pos.x), " / ", std::to_string(ship->prev_state.pos.x) });
		//console_debug({ "special alpha (", std::to_string(alpha), ") calculation: current X + prev X = ", std::to_string(currentX), " + ", std::to_string(prevX), " = ", std::to_string(ship->alpha_pos.x) });
		ship->alpha_pos.y = ship->current_state.pos.y*alpha + ship->prev_state.pos.y*(1.0 - alpha);
		return;
	}

	ship->alpha_pos.x = ship->current_state.pos.x*alpha + ship->prev_state.pos.x*(1.0 - alpha);
	ship->alpha_pos.y = ship->current_state.pos.y*alpha + ship->prev_state.pos.y*(1.0 - alpha);
}