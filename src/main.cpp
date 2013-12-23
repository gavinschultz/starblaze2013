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
#include "thrustsystem.h"
#include "motionhistory.h"

void init();
void init_ship();
void init_aliens();
void init_station();
void run();

std::unique_ptr<EntityRepository> db;
std::unique_ptr<Timer> timer;
std::unique_ptr<RenderSystem> renderer;
std::unique_ptr<PhysicsSystem> physics;
std::unique_ptr<InputSystem> input;
std::unique_ptr<ThrustSystem> thrust;

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
	thrust = std::make_unique<ThrustSystem>();

	db->registerEntity(E::playfield, {});

	renderer->init();

	init_ship();
	init_aliens();
	init_station();
}

void init_ship()
{
	const auto horient = new HorizontalOrientComponent();
	const auto radartrack = new RadarTrackableComponent();
	const auto ship_thrust = new ThrustComponent();
	const auto physical = new PhysicalComponent();
	const auto state = new TemporalState2DComponent();
	const auto body = new PoweredBodyComponent();
	const auto collision = new CollisionComponent(
	{ 0, 0, 128, 32 },
	{
		{ 0, 4, 16, 28 },
		{ 16, 8, 8, 24 },
		{ 24, 12, 8, 20 },
		{ 32, 16, 72, 16 },
		{ 104, 20, 24, 12 }
	});
	ship_thrust->max = { 6200, 1500 };
	ship_thrust->reverse_thrust_factor = 0.5;
	physical->weight = 110.0;
	physical->getDecelerationFactor = &PhysicalComponent::getShipDecelerationFactor;
	physical->box = { 0, 0, 128, 32 };
	body->phys = physical;
	body->state = state;
	body->thrust = ship_thrust;
	body->horient = horient;
	body->collision = collision;
	radartrack->phys = physical;
	radartrack->state = state;

	db->registerEntity(E::ship, { horient, body, ship_thrust, physical, state, radartrack, collision });

	motionhistory::init(renderer->getWindow().w, ship_thrust->max.x, ship_thrust->max.y);
}

void init_aliens()
{

}

void init_station()
{
	auto physical = new PhysicalComponent();
	physical->box = { 128, 64 };
	auto state = new TemporalState2DComponent();
	auto radartrack = new RadarTrackableComponent(physical, state);
	auto collision = new CollisionComponent(
	{ 0, 0, 128, 64 },
	{
		{ 0, 0, 128, 64 }
	}
	);
	auto station = new StationComponent(StationType::fuel);

	db->registerEntity(E::station, { physical, state, radartrack, collision, station });
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
		thrust->update();

		if (session::quit)
			break;

		usage::collect("input");

		if (!session::paused || session::frame_by_frame)
		{
			if (session::frame_by_frame)
				session::frame_by_frame = false;

			double dt = physics->dt;
			accumulator += lastframe_delta;
			while (accumulator >= dt)
			{
				accumulator -= dt;
				physics->update(dt);
			}
			physics->interpolate(accumulator / dt);
			usage::collect("physics");

			camera.update();
			usage::collect("camera");
		}

		renderer->draw(camera);
		usage::collect("render");

		timer->endFrame();
		usage::finish();
		usage::to_screen();
	}
}