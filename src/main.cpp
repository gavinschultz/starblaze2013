#include "stdafx.h"
#include <SDL.h>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include "timer.h"
#include <SDL_image.h>
#include <vector>
#include "Game.h"
#include <algorithm>
#include "Debug.h"
#include "Render\Renderer.h"
#include "Phys.h"
#include <math.h>
#include "Util.h"
#include "Input.h"
#include "Render\AlienSprite.h"
#include "Render\RadarSprite.h"
#include "Render\StationSprite.h"
#include "Render\ShipSprite.h"
#include "Render\BGSprite.h"
#include "Render\HUDRend.h"
#include "Render\BulletSprite.h"
#include "Render\SpriteRegister.h"
#include "Render\Renderer.h"
#include "Entity\World.h"
#include "Entity\Ship.h"
#include "Entity\Station.h"
#include "Entity\EntityRegister.h"
#include "Entity\Bullet.h"

void integrate(double delta_time, double dt);
void integrateAlpha(double alpha);

std::unique_ptr<Game> game;
std::unique_ptr<Timer> timer;
std::unique_ptr<Renderer> renderer;
std::unique_ptr<World> world;
std::unique_ptr<Debug> debug;

void run();

int main(int argc, char* args[])
{
	int retcode = 0;
	try
	{
		run();
	}
	catch (std::runtime_error re)
	{
		console_debug({ "Unhandled run-time error: ", re.what() });
		retcode = 6;
	}
	catch (std::exception e)
	{
		console_debug({ "Unhandled unknown exception: ", e.what() });
		retcode = 7;
	}
	SDL_Quit();
	return retcode;
}

void run()
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

	Camera camera = Camera(
		SDL_Rect{ 0, 0, renderer->window.w, renderer->window.h }, 
		SDL_Rect{ world->ship_limits.x * renderer->scaling, 0, world->ship_limits.w * renderer->scaling, world->ship_limits.h * renderer->scaling }
	);

	// Load assets
	Ship* s = new Ship();
	Station* station = new Station();

	game->entity_register.registerEntity(s);
	for (int i = 0; i < 5; i++)
	{
		Alien* alien = new Alien();
		game->entity_register.registerEntity(alien);
		renderer->sprite_register.registerSprite(new AlienSprite(renderer.get()), alien);
	}
	game->entity_register.registerEntity(station);
	for (int i = 0; i < 20; i++)
	{
		Bullet* bullet = new Bullet();
		game->entity_register.registerEntity(bullet);
		renderer->sprite_register.registerSprite(new BulletSprite(renderer.get()), bullet);
	}

	renderer->sprite_register.registerBackground(new BGSprite(renderer.get()));
	renderer->sprite_register.registerSprite(new StationSprite(renderer.get(), *station), station);
	renderer->sprite_register.registerShipSprite(new ShipSprite(renderer.get()));
	renderer->sprite_register.registerSprite(new RadarSprite(renderer.get()));
	renderer->sprite_register.registerHUD(new HUDRend(renderer.get()));
	renderer->sprite_register.registerRadar(new RadarSprite(renderer.get()));
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
	ship->current_state.pos.x = world->ship_limits.x;
	ship->current_state.pos.y = world->ship_limits.y + world->ship_limits.h - ship->bounding_box.h;

	station->current_state.pos.y = world->ship_limits.y + world->ship_limits.h - station->bounding_box.h;

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
		debug->set("thrust_multiplier", thrust_multiplier);

		debug->setMotionRecordMaxThresholds(ship->max_thrust.x, ship->max_thrust.y);
		debug->addMotionRecord(ship->current_state.thrust.x, ship->current_state.thrust.y);

		if (ship->current_state.thrust.x > ship->max_thrust.x * thrust_multiplier)
			ship->current_state.thrust.x = ship->max_thrust.x * thrust_multiplier;
		else if (ship->current_state.thrust.x < -ship->max_thrust.x * thrust_multiplier)
			ship->current_state.thrust.x = -ship->max_thrust.x * thrust_multiplier;
		// end calculate thrust

		// alien AI
		for (auto& alien : game->entity_register.getAliens())
		{
			alien->runAI(ship, dt);
		}
		// end alien AI

		if (!game->is_paused || game->is_frame_by_frame)
		{
			if (game->is_frame_by_frame)
				game->is_frame_by_frame = false;

			accumulator += delta_time;
			while (accumulator >= dt)
			{
				//ship->prev_state = ship->current_state;
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
}

void integrate(double delta_time, double dt)
{
	for (auto& entity : game->entity_register.getAll())
	{
		if (!entity->is_active)
			continue;

		entity->prev_state = entity->current_state;
		entity->current_state.acc.x = entity->current_state.thrust.x / entity->weight;
		entity->current_state.vel.x += entity->current_state.acc.x * dt;
		entity->current_state.vel.y += entity->current_state.thrust.y;	// TODO: acceleration

		// decceleration
		entity->current_state.vel.x -= entity->current_state.vel.x * (entity->getDecelerationFactorX() * dt);
		entity->current_state.vel.y -= entity->current_state.vel.y * (entity->getDecelerationFactorY() * dt);

		if (entity->current_state.vel.y > entity->max_lift) entity->current_state.vel.y = entity->max_lift;
		if (entity->current_state.vel.y < -entity->max_lift) entity->current_state.vel.y = -entity->max_lift;

		entity->current_state.pos.x += (entity->current_state.vel.x * dt) + (entity->current_state.acc.x * 0.5 * dt * dt);
		entity->current_state.pos.y += entity->current_state.vel.y * dt; // TODO: acceleration

		entity->altitude = std::max(0.0, world->ship_limits.y + world->ship_limits.h - entity->current_state.pos.y - entity->bounding_box.h);
		if (entity->altitude == 0.0)
		{
			entity->current_state.pos.y = world->ship_limits.y + world->ship_limits.h - entity->bounding_box.h;
			entity->current_state.vel.y = 0.0;
		}

		if (entity->current_state.pos.y < world->ship_limits.y)
		{
			entity->current_state.pos.y = world->ship_limits.y;
			entity->current_state.vel.y = 0.0;
		}

		if (entity->current_state.pos.x > world->w)
		{
			entity->current_state.pos.x -= world->w;
			entity->current_state.loop_count++;
		}
		else if (entity->current_state.pos.x < 0.0)
		{
			entity->current_state.pos.x += world->w;
			entity->current_state.loop_count--;
		}

		if (entity->weight == 0.5)
		{
			console_debug({ "bullet.x: ", std::to_string(entity->current_state.pos.x) });
		}
	}
}

void integrateAlpha(double alpha)
{
	for (auto& entity : game->entity_register.getAll())
	{
		if (entity->current_state.loop_count != entity->prev_state.loop_count)
		{
			double wrap_factor = ((entity->current_state.loop_count - entity->prev_state.loop_count)*world->w);
			double currentX = entity->current_state.pos.x*alpha;
			double prevX = (entity->prev_state.pos.x - wrap_factor)*(1.0 - alpha);
			entity->alpha_pos.x = currentX + prevX;
		}
		else
		{
			entity->alpha_pos.x = entity->current_state.pos.x*alpha + entity->prev_state.pos.x*(1.0 - alpha);
		}
		entity->alpha_pos.y = entity->current_state.pos.y*alpha + entity->prev_state.pos.y*(1.0 - alpha);
	}
}