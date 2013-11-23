#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_image.lib")
#pragma comment(lib, "SDL2_ttf.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")

#include <SDL.h>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include "timer.h"
#include <SDL_image.h>
#include <vector>
#include "Entity.h"
#include "Game.h"
#include <algorithm>
#include "Debug.h"
#include "Render.h"
#include "Phys.h"
#include <math.h>
#include "Util.h"
#include "Input.h"

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

	Input input = Input();
	world = std::unique_ptr<World>{new World(100)};
	game = std::unique_ptr<Game>{new Game()};
	timer = std::unique_ptr<Timer>{new Timer()};
	renderer = std::unique_ptr<Renderer>{new Renderer(1024, 768, 4, world->w)};
	debug = std::unique_ptr<Debug>{new Debug()};
#ifdef _DEBUG
	renderer->toggleGrid(true);
	//renderer->toggleMotionHistory(true);
#endif

	game->ship_limits = { 48, 0, 160, 144 };
	Camera camera = Camera(
		SDL_Rect{ 0, 0, renderer->window.w, renderer->window.h }, 
		SDL_Rect{ game->ship_limits.x * renderer->scaling, 0, game->ship_limits.w * renderer->scaling, game->ship_limits.h * renderer->scaling }
	);

	// Load assets
	BGSprite* bg_sprite = new BGSprite(renderer.get(), world.get());
	renderer->sprite_register.registerSprite(bg_sprite);

	renderer->sprite_register.registerSprite(new StationSprite(renderer.get()));

	Ship* s = new Ship();
	s->direction = ShipDirection::right;
	s->bounding_box = { 0, 0, 32, 8 };	// TODO: scaling real-time
	game->entity_register.registerEntity(s);
	ShipSprite* shipSprite = new ShipSprite(renderer.get(), s);
	renderer->sprite_register.registerSprite(shipSprite);

	auto a = new Alien();
	a->bounding_box = { 0, 0, 16, 12 };
	auto alien = game->entity_register.registerEntity(a);
	AlienSprite* alienSprite = new AlienSprite(renderer.get(), alien);
	renderer->sprite_register.registerSprite(alienSprite);

	renderer->sprite_register.registerSprite(new RadarSprite(renderer.get()));
	// end load assets

	auto title_plate = std::shared_ptr<TextPlate>{ new TextPlate{ {
		{ "STAR BLAZE", { 48 * 4, 0 } },
		{ "BY GREG ZUMWALT", { 8 * 4, 16 * 4 } },
		{ "COPYRIGHT 1983", { 18 * 4, 40 * 4 } },
		{ "LICENSED TO", { 42 * 4, 52 * 4 } },
		{ "TANDY CORP", { 50 * 4, 64 * 4 } },
		{ "ALL RIGHTS", { 50 * 4, 76 * 4 } },
		{ "RESERVED", { 66 * 4, 88 * 4 } }
	} } };

	game->start();

	SDL_SetRelativeMouseMode(SDL_TRUE);

	Ship* ship = game->entity_register.getShip();
	ship->current_state.pos.x = game->ship_limits.x;
	ship->current_state.pos.y = game->ship_limits.y + game->ship_limits.h - ship->bounding_box.h;

	alien->current_state.pos.x = ship->current_state.pos.x + 50;
	alien->current_state.pos.y = ship->current_state.pos.y - 100;

	double accumulator = 0;
	const double dt = 1.0 / 60.0;

	while (true)
	{
		timer->startFrame();
		double time_start_frame = timer->getTime();

		auto delta_time = timer->getLastFrameDuration();
		if (delta_time > 0.25)
		{
			console_debug({ "Delta exceeded max" });
			delta_time = 0.25;
		}

		ship->current_state.thrust.x = 0.0;
		ship->current_state.thrust.y = 0.0;

		input.handleInput();

		if (game->quit)
			break;

		// Apply additional reverse thrust if required
		double thrust_multiplier = 1.0;
		if ((ship->current_state.vel.x > 0.0 && ship->current_state.thrust.x < 0.0) || (ship->current_state.vel.x < 0.0 && ship->current_state.thrust.x > 0.0))
			thrust_multiplier = ship->reverse_thrust_factor;

		ship->current_state.thrust.x *= thrust_multiplier;

		debug->setMotionRecordMaxThresholds(ship->max_thrust.x, ship->max_thrust.y);
		debug->addMotionRecord(ship->current_state.thrust.x, ship->current_state.thrust.y);

		if (ship->current_state.thrust.x > ship->max_thrust.x * thrust_multiplier)
			ship->current_state.thrust.x = ship->max_thrust.x * thrust_multiplier;
		else if (ship->current_state.thrust.x < -ship->max_thrust.x * thrust_multiplier)
			ship->current_state.thrust.x = -ship->max_thrust.x * thrust_multiplier;
		// end calculate thrust

		if (!game->is_paused || game->is_frame_by_frame)
		{
			if (game->is_frame_by_frame)
				game->is_frame_by_frame = false;

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
			camera.prev_focus_rect = camera.focus_rect;
			camera.focus_point = { std::lround(ship->alpha_pos.x * renderer->scaling), std::lround(ship->alpha_pos.y * renderer->scaling) };
			camera.focus_loop_count = ship->current_state.loop_count;

			camera.focus_point_vel.x = camera.focus_point.x - camera.prev_focus_point.x + ((camera.focus_loop_count - camera.prev_focus_loop_count) * renderer->width);

			int32_t turn_speed;
			if ((camera.focus_point_vel.x < 0 && ship->direction == ShipDirection::right) || (camera.focus_point_vel.x > 0 && ship->direction == ShipDirection::left))
				turn_speed = 6L;
			else
				turn_speed = (int32_t)std::max(std::abs(lround(camera.focus_point_vel.x * 0.4)), 8L);
			int32_t max_velocity;
			if (ship->direction == ShipDirection::right)
				max_velocity = camera.focus_point_vel.x + turn_speed;
			else
				max_velocity = camera.focus_point_vel.x - turn_speed;
			int32_t desired_x_abs;
			int32_t proposed_x_abs = camera.prev_focus_rect.x + max_velocity;  // NOTE: might fail to be absolute once we can have dir = right and negative velocity

			if (ship->direction == ShipDirection::right)
			{
				desired_x_abs = camera.focus_point.x;
				if (desired_x_abs < 0)
					desired_x_abs += renderer->width;
				if (renderer->isRightOf(desired_x_abs, proposed_x_abs))
					camera.focus_rect.x = proposed_x_abs;
				else
					camera.focus_rect.x = desired_x_abs;
			}
			else
			{
				desired_x_abs = camera.focus_point.x + ship->bounding_box.w*renderer->scaling - camera.focus_rect.w;
				if (desired_x_abs < 0)
					desired_x_abs += renderer->width;
				if (!renderer->isRightOf(desired_x_abs, proposed_x_abs))
					camera.focus_rect.x = proposed_x_abs;
				else
					camera.focus_rect.x = desired_x_abs;
			}

			if (camera.focus_rect.x < 0)
			{
				camera.focus_rect.x += renderer->width;
			}
			else if (camera.focus_rect.x > world->w * renderer->scaling)
			{
				camera.focus_rect.x -= renderer->width;
			}
			camera.view_rect.x = camera.focus_rect.x - (48 * renderer->scaling);
			if (camera.view_rect.x < 0)
				camera.view_rect.x += renderer->width;
		}

		double time_before_render = timer->getTime();
		double usage_game = util::round(((time_before_render - time_start_frame) / delta_time), 3) * 100.0;

		//renderer->renderTextPlate(title_plate);

		renderer->render(&camera);

		double time_after_render = timer->getTime();
		double render_time = util::round(((time_after_render - time_before_render) / delta_time), 3) * 100.0;

		if (timer->getTotalFrames() % 10 == 0)
		{
			debug->set("% Usage (game)", std::to_string(usage_game).substr(0, 4));
			debug->set("% Usage (render)", std::to_string(render_time).substr(0, 4));
		}

		timer->endFrame();
	}

	SDL_Quit();
	return 0;
}

void integrate(double delta_time, double dt)
{
	Ship* ship = game->entity_register.getShip();

	double accel = ship->current_state.thrust.x / ship->weight;
	double vel = ship->current_state.vel.x + accel * dt;

	// decceleration X
	double deccelleration_factor;
	if (std::abs(ship->current_state.vel.x) > 200 || std::abs(ship->current_state.thrust.x) > 0)
		deccelleration_factor = 0.4;
	else if (std::abs(ship->current_state.vel.x) > 50) // slow more when already slow and no thrust
		deccelleration_factor = 1.1;
	else
		deccelleration_factor = 2.3;
	vel -= ship->current_state.vel.x * (deccelleration_factor * dt);
	// decceleration Y
	ship->current_state.vel.y -= ship->current_state.vel.y * (4.0 * dt);

	double dist = (ship->current_state.vel.x * dt) + (accel * 0.5 * dt * dt);


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
}

void integrateAlpha(double alpha)
{
	Ship* ship = game->entity_register.getShip();

	if (ship->current_state.loop_count != ship->prev_state.loop_count)
	{
		double wrap_factor = ((ship->current_state.loop_count - ship->prev_state.loop_count)*world->w);
		double currentX = ship->current_state.pos.x*alpha;
		double prevX = (ship->prev_state.pos.x - wrap_factor)*(1.0 - alpha);
		ship->alpha_pos.x = currentX + prevX;
	}
	else
	{
		ship->alpha_pos.x = ship->current_state.pos.x*alpha + ship->prev_state.pos.x*(1.0 - alpha);
	}
	ship->alpha_pos.y = ship->current_state.pos.y*alpha + ship->prev_state.pos.y*(1.0 - alpha);

	Alien* alien = game->entity_register.getAlien();
	alien->alpha_state = alien->current_state.pos;
}