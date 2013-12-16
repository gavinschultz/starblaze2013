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
#include "prefs.h"

void run();
void init();

std::unique_ptr<EntityRepository> db;
std::unique_ptr<Timer> timer;
std::unique_ptr<RenderSystem> renderer;
std::unique_ptr<PhysicsSystem> physics;
std::unique_ptr<InputSystem> input;

int main(int argc, char* args[])
{
	try
	{
		init();
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

void init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		program::exit(RetCode::sdl_error, { "Error initializing SDL: ", SDL_GetError() });
	}

	db = std::make_unique<EntityRepository>();
	renderer = std::make_unique<RenderSystem>(1366, 768, 4, 0);
	timer = std::make_unique<Timer>();
	physics = std::make_unique<PhysicsSystem>();
	input = std::make_unique<InputSystem>();

	const auto horient = new HorizontalOrientComponent();
	const auto state = new TemporalState2DComponent();
	const auto thrust = new ThrustComponent();
	thrust->max = { 400, 12 };
	const auto radartrack = new RadarTrackableComponent();
	db->registerEntity(E::ship, { horient, state, thrust, radartrack });
	db->registerEntity(E::playfield, {});

	renderer->init();
}

void run()
{
	PlayField* playfield = db->getPlayField();
	auto camera = Camera{ SDL_Rect{ 0, 0, 1366, 768 }, SDL_Rect{ (int)playfield->boundaries.x, (int)playfield->boundaries.y, (int)playfield->boundaries.w, (int)playfield->boundaries.h } };

	double accumulator = 0.0;
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

		double dt = physics->dt;
		accumulator += lastframe_delta;
		while (accumulator >= dt)
		{
			accumulator -= dt;
			physics->update(dt);
		}
		physics->interpolate(accumulator / dt);

		usage::collect("logic");

		renderer->draw(camera);
		usage::collect("render");

		timer->endFrame();
		usage::finish();
		usage::to_screen();
	}
}