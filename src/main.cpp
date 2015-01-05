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
void init_bullets();
void run();

std::unique_ptr<EntityRepository> db;
std::unique_ptr<Timer> timer;
std::unique_ptr<RenderSystem> renderer;
std::unique_ptr<PhysicsSystem> physics;
std::unique_ptr<InputSystem> input;
std::unique_ptr<ThrustSystem> thrust;

#include <sstream>

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

	Window window = { 1366, 768 };
	db->registerPlayField(window);
	renderer = std::make_unique<RenderSystem>(window);
	timer = std::make_unique<Timer>();
	physics = std::make_unique<PhysicsSystem>();
	input = std::make_unique<InputSystem>();
	thrust = std::make_unique<ThrustSystem>();

	init_bullets();
	init_ship();
	init_aliens();
	init_station();

	debug::console({ db->getInfo() });
}

void init_ship()
{
	const auto horient = new HorizontalOrientComponent();
	const auto thrust = new ThrustComponent({ 0, 0 }, 1.0F, true);
	const auto physical = new PhysicalComponent();
	const auto state = new TemporalState2DComponent();
	const auto radartrack = new RadarTrackableComponent();
	const auto fire = new FireBulletsComponent();
	const auto player = new PlayerComponent();
	const auto collision = new CollisionComponent(
	{ 0, 0, 128, 32 },
	{
		{ 0, 4, 16, 28 }, // tail end
		{ 16, 8, 8, 24 },
		{ 24, 12, 8, 20 },
		{ 32, 16, 72, 16 },
		{ 104, 20, 24, 12 }
	});
	const auto body = new PoweredBodyComponent(state, thrust, physical, horient, collision);
	state->ignore_acc.y = true;	// don't consider y acceleration when determining y velocity - may be less realistic but feels better
	thrust->max = { 6200, 2800 };
	thrust->reverse_thrust_factor = 0.9;
	physical->weight = 3.0;
	physical->getDecelerationFactor = &PhysicalComponent::getShipDecelerationFactor;
	physical->box = { 0, 0, 128, 32 };

	// Get ID of first bullet
	auto bullet_lower_id = db->getEntitiesOfType(E::ebullet)[0];
	fire->registerBullets(bullet_lower_id);

	auto id = db->registerEntity(E::eship);
	db->registerComponent(horient, id);
	db->registerComponent(body, id);
	db->registerComponent(thrust, id);
	db->registerComponent(physical, id);
	db->registerComponent(state, id);
	db->registerComponent(radartrack, id);
	db->registerComponent(collision, id);
	db->registerComponent(fire, id);
	db->registerComponent(player, id);

	motionhistory::init(renderer->getWindow().w, thrust->max.x, thrust->max.y);
}

void init_aliens()
{
	for (int i = 0; i < 10; i++)
	{
		auto physical = new PhysicalComponent();
		physical->box = { 0, 0, 64, 48 };
		physical->weight = 110.0;
		auto thrust = new ThrustComponent({ 6200, 1500 }, 1.0);
		auto state = new TemporalState2DComponent();
		state->current.pos.x = i * 64;
		state->current.pos.y = i * 48;
		auto radartrack = new RadarTrackableComponent();
		auto collision = new CollisionComponent(
		{ 0, 0, 64, 48 },
		{
			{ 24, 0, 16, 36 },
			{ 8, 8, 48, 12 },
			{ 16, 4, 32, 20 },
			{ 0, 12, 64, 4 },
			{ 0, 28, 64, 8 },
			{ 0, 36, 24, 12 },
			{ 40, 36, 24, 12 }
		});
		auto body = new PoweredBodyComponent(state, thrust, physical, nullptr, collision);
		auto id = db->registerEntity(E::ealien);
		db->registerComponent(physical, id);
		db->registerComponent(state, id);
		db->registerComponent(radartrack, id);
		db->registerComponent(collision, id);
	}
}

void init_station()
{
	auto physical = new PhysicalComponent();
	physical->box = { 0, 0, 128, 64 };
	auto state = new TemporalState2DComponent();
	auto playarea = db->getPlayField()->getPlayArea(physical->box);
	state->current.pos.y = playarea.h;
	auto radartrack = new RadarTrackableComponent();
	auto collision = new CollisionComponent{
		{ 0, 0, 128, 64 },
		{
			{ 0, 0, 128, 64 }
		}
	};
	auto station = new StationComponent(StationType::fuel);

	auto id = db->registerEntity(E::estation);
	db->registerComponent(physical, id);
	db->registerComponent(state, id);
	db->registerComponent(radartrack, id);
	db->registerComponent(collision, id);
	db->registerComponent(station, id);
}

void init_bullets()
{
	for (int i = 0; i < 30; i++)
	{
		auto thrust = new ThrustComponent{ { 6000.0, 0.0 }, { 0.0 } };
		auto state = new TemporalState2DComponent{};
		auto physical = new PhysicalComponent{ { 0.0, 0.0, 16.0, 8.0 }, 1.0 };
		auto collision = new CollisionComponent{
			{ 0.0, 0.0, 16.0, 8.0 },
			{
				{ 0.0, 0.0, 16.0, 8.0 }
			}
		};
		auto lifetime = new LifetimeComponent{ 0.7f };

		auto id = db->registerEntity(E::ebullet);
		db->registerComponent(state, id);
		db->registerComponent(thrust, id);
		db->registerComponent(physical, id);
		db->registerComponent(collision, id);
		db->registerComponent(lifetime, id);
	}
}

void run()
{
	PlayField* playfield = db->getPlayField();
	auto camera = Camera{ SDL_Rect{ 0, 0, 1366, 768 }, SDL_Rect{ (int)playfield->boundaries.x, (int)playfield->boundaries.y, (int)playfield->boundaries.w, (int)playfield->boundaries.h } };

	float accumulator = 0.0;
	while (true)
	{
		/* debug */
		auto& db1 = *db.get();
		db->debug_getcomponentsoftype_calls = 0;
		db->debug_getcomponentsoftypeforentity_calls = 0;
		db->debug_getentitiesoftype_calls = 0;
		db->debug_getentitieswithcomponent_calls = 0;

		timer->startFrame();
		usage::start();

		float lastframe_delta = timer->getLastFrameDuration();
		if (lastframe_delta > 0.25)
		{
			debug::console({ "Warning, excessive delta time from last frame (", std::to_string(lastframe_delta), ")" });
			lastframe_delta = 0.25;
		}

		input->update();

		if (session::quit)
			break;

		usage::collect("input");

		if (!session::paused || session::frame_by_frame)
		{
			thrust->update();

			if (session::frame_by_frame)
				session::frame_by_frame = false;

			float dt = physics->dt;
			accumulator += lastframe_delta;
			while (accumulator >= dt)
			{
				accumulator -= dt;
				physics->update(dt);
				physics->collide();
			}
			physics->interpolate(accumulator / dt);
			usage::collect("physics");

			camera.update();
			usage::collect("camera");
		}

		//debug::set("getcomponentsoftype", db->debug_getcomponentsoftype_calls); // 3/frame
		//debug::set("getentitiesoftype", db->debug_getentitiesoftype_calls); // 28/frame
		//debug::set("getentitieswithcomponent", db->debug_getentitieswithcomponent_calls); // 5/frame
		//debug::set("getcomponentsoftypeforentity", db->debug_getcomponentsoftypeforentity_calls);	// 1811/frame

		renderer->draw(camera);
		usage::collect("render");

		renderer->flip();
		usage::collect("vsync");

		timer->endFrame();
		usage::finish();
		usage::to_screen();
	}
}