#include "stdafx.h"
#include <stdexcept>
#include "program.h"
#include "timer.h"
#include "render\rendersystem.h"
#include "input.h"
#include "physics.h"
#include "component.h"
#include "game.h"

void run();

std::unique_ptr<Game> game;
std::unique_ptr<EntityRepository> db;
std::unique_ptr<Timer> timer;

int main(int argc, char* args[])
{
	try
	{
		run();
	}
	catch (std::runtime_error re)
	{
		program::exit(RetCode::runtime_error, { "Unhandled run-time error: ", re.what() });
	}
	catch (std::exception e)
	{
		program::exit(RetCode::unknown_error, { "Unhandled unknown exception: ", e.what() });
	}

	program::cleanup();
	return RetCode::success;
}

void run()
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		program::exit(RetCode::sdl_error, { "Error initializing SDL: ", SDL_GetError() });
	}

	game = std::make_unique<Game>();
	db = std::make_unique<EntityRepository>();
	timer = std::make_unique<Timer>();
	auto render = std::make_unique<RenderSystem>(1366, 768, 4, 0);
	auto physics = std::make_unique<PhysicsSystem>();
	auto input = std::make_unique<InputSystem>();

	while (true)
	{
		timer->startFrame();
		usage::start();

		double lastframe_delta = timer->getLastFrameDuration();
		if (lastframe_delta > 0.25)
		{
			debug::console({ "Warning, excessive delta time from last frame (", std::to_string(lastframe_delta), ")" });
			lastframe_delta = 0.25;
		}

		input->update();

		if (game->quit)
			break;
		if (game->is_paused)
			continue;

		physics->update();
		usage::collect("logic");

		render->update();
		usage::collect("render");

		timer->endFrame();
		usage::finish();
		usage::to_screen();
	}
}