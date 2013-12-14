#include "stdafx.h"
#include <stdexcept>
#include "program.h"
#include "timer.h"
#include "render\rendersystem.h"
#include "input.h"
#include "physics.h"
#include "component.h"
#include "session.h"
#include "playfield.h"
#include "render\camera.h"

void run();

std::unique_ptr<EntityRepository> db;
std::unique_ptr<Timer> timer;
std::unique_ptr<PlayField> playfield;

int main(int argc, char* args[])
{
	try
	{
		run();
	}
	catch (std::runtime_error re)
	{
		program::exit(RetCode::runtime_error, { "Run-time error: ", re.what() });
	}
	catch (std::exception e)
	{
		program::exit(RetCode::unknown_error, { "Unknown exception: ", e.what() });
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

	auto render = std::make_unique<RenderSystem>(1366, 768, 4, 0);
	auto physics = std::make_unique<PhysicsSystem>();
	auto input = std::make_unique<InputSystem>();
	db = std::make_unique<EntityRepository>();
	timer = std::make_unique<Timer>();
	playfield = std::make_unique<PlayField>(render->getWindow());

	auto camera = Camera{ SDL_Rect{ 0, 0, 1366, 768 }, SDL_Rect{ (int)playfield->boundaries.x, (int)playfield->boundaries.y, (int)playfield->boundaries.w, (int)playfield->boundaries.h } };

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

		if (session::quit)
			break;
		if (session::is_paused)
			continue;

		physics->update();
		usage::collect("logic");

		if (timer->getTotalFrames() % 5 == 0)
			debug::set("Physics time", timer->getTime() - timer->getFrameStartTime());
		debug::set("Total frames", timer->getTotalFrames());

		render->draw(camera);
		usage::collect("render");

		timer->endFrame();
		usage::finish();
		usage::to_screen();
	}
}