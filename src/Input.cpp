#include "stdafx.h"
#include <algorithm>
#include <SDL.h>
#include "input.h"
#include "component.h"
#include "session.h"
#include "mathutil.h"

#include "timer.h"

class InputSystem::impl
{
public:
	SDL_GameController* controller;
	void resetComponentsForInput();
	void handleKeyboardEvent(const SDL_KeyboardEvent& e) const;
	void handleKeyboardState() const;
	void handleMouseMotionEvent(const SDL_MouseMotionEvent& e) const;
	void handleJoystickState() const;
	void turnWithThrustConsideration(HOrient requested, HOrient& current, ThrustComponent& thrust) const;
};

InputSystem::InputSystem() : pi{ new impl{} }
{
	detect();
}

InputSystem::~InputSystem() {}

void InputSystem::update()
{
	if (timer->getTotalFrames() % 180 == 0)	// joystick detection in SDL seems to be cheap, but still no need to hammer it
		detect();

	pi->resetComponentsForInput();

	SDL_Event pevent;
	while (SDL_PollEvent(&pevent))
	{
		if (session::paused && pevent.type != SDL_KEYDOWN)
			continue;

		switch (pevent.type)
		{
		case SDL_KEYDOWN:
			session::handleKeyboardEvent(pevent.key);
			pi->handleKeyboardEvent(pevent.key);
			break;
		case SDL_MOUSEMOTION:
			pi->handleMouseMotionEvent(pevent.motion);
			break;
		default:
			break;
		}
	}

	pi->handleJoystickState();
	pi->handleKeyboardState();
}

void InputSystem::impl::resetComponentsForInput()
{
	auto ship_id = db->getEntitiesOfType(E::eship)[0];
	auto body = db->getComponentOfTypeForEntity<PoweredBodyComponent>(ship_id);
	auto fire = db->getComponentOfTypeForEntity<FireBulletsComponent>(ship_id);
	if (body)
		body->thrust->resetCurrent();
}

void InputSystem::detect()
{
	if (SDL_GameControllerGetAttached(pi->controller))
		return;

	pi->controller = nullptr;

	int joystick_count;
	if ((joystick_count = SDL_NumJoysticks()) < 0)
	{
		debug::console({ "Unable to determine number of connected joysticks: ", SDL_GetError() });
		return;
	}

	for (int i = 0; i < joystick_count; i++)
	{
		if (SDL_IsGameController(i))
		{
			auto* opened_controller = SDL_GameControllerOpen(i);
			if (!opened_controller)
			{
				debug::console({ "Could not open controller: ", SDL_GetError() });
				return;
			}
			debug::console({ "Controller detected: ", SDL_GameControllerName(opened_controller) });
			debug::console({ "Controller mapping: ", SDL_GameControllerMapping(opened_controller) });
			pi->controller = opened_controller;
			break;
		}
	}
}

void InputSystem::impl::handleKeyboardEvent(const SDL_KeyboardEvent& e) const
{
	if (session::paused && !session::frame_by_frame)
		return;

	auto ship_id = db->getEntitiesOfType(E::eship)[0];
	auto body = db->getComponentOfTypeForEntity<PoweredBodyComponent>(ship_id);
	auto thrust = db->getComponentOfTypeForEntity<ThrustComponent>(ship_id);
	auto fire = db->getComponentOfTypeForEntity<FireBulletsComponent>(ship_id);

	auto* keystate = SDL_GetKeyboardState(NULL);
	switch (e.keysym.sym)
	{
	case SDLK_F6:
		//game->mouse_sensitivity = std::max(2.0f, game->mouse_sensitivity - 0.5f);
		break;
	case SDLK_F7:
		//game->mouse_sensitivity = std::min(6.0f, game->mouse_sensitivity + 0.5f);
		break;
	case SDLK_F10:
		//renderer->toggleMotionHistory(!renderer->is_motionhistory_visible);
		break;
	case SDLK_SPACE:
		if (fire)
		{
			fire->fire();
		}
		break;
	case SDLK_t:
		if (!thrust)
			return;

		if (keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT])
		{
			thrust->max.x -= 200;
			thrust->max.y -= 200;
		}
		else
		{
			thrust->max.x += 200;
			thrust->max.y += 200;
		}
		break;
	}
}

void InputSystem::impl::handleKeyboardState() const
{
	if (session::paused && !session::frame_by_frame)
		return;

	auto ship_id = db->getEntitiesOfType(E::eship)[0];
	auto body = db->getComponentOfTypeForEntity<PoweredBodyComponent>(ship_id);
	auto thrust = db->getComponentOfTypeForEntity<ThrustComponent>(ship_id);
	auto orient = db->getComponentOfTypeForEntity<HorizontalOrientComponent>(ship_id);

	if (!thrust)
		return;

	auto* keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_UP])
		thrust->current.y = -thrust->max.y;
	if (keystate[SDL_SCANCODE_DOWN])
		thrust->current.y = thrust->max.y;
	if (keystate[SDL_SCANCODE_LEFT])
	{
		thrust->current.x = -thrust->max.x;
		turnWithThrustConsideration(HOrient::left, orient->direction, *thrust);
	}
	if (keystate[SDL_SCANCODE_RIGHT])
	{
		thrust->current.x = thrust->max.x;
		turnWithThrustConsideration(HOrient::right, orient->direction, *thrust);
	}
}

void InputSystem::impl::handleMouseMotionEvent(const SDL_MouseMotionEvent& e) const
{
	if (session::paused && !session::frame_by_frame)
		return;

	//// Ship control via the mouse is kinda shitty, only because I haven't found a way to easily get perpetual thrust in a single direction
	//// Needs some experimentation - probably should try non-relative coordinates next
	//if (!game->is_paused)
	//{
	//	if (std::abs(e.xrel) > 2)
	//		ship->state.current.thrust.x = util::getsign(e.xrel) * ship->max_thrust.x;
	//	else
	//		ship->state.current.thrust.x = e.xrel * game->mouse_sensitivity * 16.0;
	//	ship->state.current.thrust.y = e.yrel * game->mouse_sensitivity;
	//	if (e.xrel > 0)
	//		this->turnShip(ship, ShipDirection::right);
	//	else if (e.xrel < 0)
	//		this->turnShip(ship, ShipDirection::left);
	//}
}

void InputSystem::impl::handleJoystickState() const
{
	if (session::paused && !session::frame_by_frame)
		return;
	if (!SDL_GameControllerGetAttached(controller))
		return;

	auto ship_id = db->getEntitiesOfType(E::eship)[0];
	auto thrust = db->getComponentOfTypeForEntity<ThrustComponent>(ship_id);
	auto orient = db->getComponentOfTypeForEntity<HorizontalOrientComponent>(ship_id);
	auto fire = db->getComponentOfTypeForEntity<FireBulletsComponent>(ship_id);

	if (thrust)
	{
		const static int16_t tolerance_x = 6000;
		const static int16_t tolerance_y = 8000;
		const float x_modifier = thrust->max.x / (std::numeric_limits<int16_t>::max() - tolerance_x);
		const float y_modifier = thrust->max.y / (std::numeric_limits<int16_t>::max() - tolerance_y);

		int16_t left_x_raw = SDL_GameControllerGetAxis(controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
		int16_t left_y_raw = SDL_GameControllerGetAxis(controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
		int16_t left_x = mathutil::getsign(left_x_raw)*std::max(0, (std::abs(left_x_raw) - tolerance_x));
		int16_t left_y = mathutil::getsign(left_y_raw)*std::max(0, (std::abs(left_y_raw) - tolerance_y));
		if (left_x != 0)
		{
			thrust->current.x = left_x * x_modifier;
			auto requested_direction = (left_x > 0 ? HOrient::right : HOrient::left);
			turnWithThrustConsideration(requested_direction, orient->direction, *thrust);
		}
		if (std::abs(left_y) > 0)
		{
			thrust->current.y = left_y * y_modifier;
		}
	}

	if (fire)
	{
		if (fire && SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X))
		{
			fire->fire();
		}
	}
}

void InputSystem::impl::turnWithThrustConsideration(HOrient requested, HOrient& current, ThrustComponent& thrust) const
{
	if (requested == current)
		return;
	thrust.current.x = 0;	// one frame to turn without modifying thrust - allow for a turn without necessarily reducing existing momentum
	current = requested;
}