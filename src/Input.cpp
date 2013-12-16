#include "stdafx.h"
#include <algorithm>
#include <SDL.h>
#include "input.h"
#include "component.h"
#include "session.h"

SDL_GameController* _controller = nullptr;

class InputSystem::impl
{
public:
	SDL_GameController* controller;
	void resetComponentsForInput();
	void connectController();
	void handleKeyboardEvent(const SDL_KeyboardEvent& e) const;
	void handleKeyboardState() const;
	void handleMouseMotionEvent(const SDL_MouseMotionEvent& e) const;
	void handleJoystickState() const;
	void turnWithThrustConsideration(HOrient requested, HOrient& current, ThrustComponent& thrust) const;
};

InputSystem::InputSystem() : pi{ new impl{} }
{
	pi->connectController();
}

InputSystem::~InputSystem() {}

void InputSystem::update()
{
	pi->resetComponentsForInput();

	SDL_Event pevent;
	while (SDL_PollEvent(&pevent))
	{
		if (session::is_paused && pevent.type != SDL_KEYDOWN)
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

	if (session::is_paused)
		return;

	pi->handleJoystickState();
	pi->handleKeyboardState();
}

void InputSystem::impl::resetComponentsForInput()
{
	auto thrust = (ThrustComponent*)db->getComponentsOfTypeForEntity(E::ship, C::thrust);
	thrust->current.x = 0;
}

void InputSystem::impl::connectController()
{
	if (this->controller)
		return;	// already connected

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
			controller = opened_controller;
			break;
		}
	}
}

void InputSystem::impl::handleKeyboardEvent(const SDL_KeyboardEvent& e) const
{
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
	case SDLK_F11:
		//renderer->toggleGrid(!renderer->is_grid_visible);
		break;
	case SDLK_p:
		//game->togglePause(!game->is_paused);
		break;
	case SDLK_f:
		//game->advanceFrameByFrame();
		break;
	case SDLK_SPACE:
		//if (ship)
		//{
		//	ship->fire();
		//}
		break;
	case SDLK_RETURN:
		//if (e.keysym.mod & KMOD_ALT)
		//	renderer->toggleFullscreen(!renderer->is_fullscreen);
		break;
	}
}

void InputSystem::impl::handleKeyboardState() const
{
	auto thrust = (ThrustComponent*)db->getComponentsOfTypeForEntity(E::ship, C::thrust);
	auto orient = (HorizontalOrientComponent*)db->getComponentsOfTypeForEntity(E::ship, C::horient);

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

	debug::set("thrust", thrust->current.x);
}

void InputSystem::impl::handleMouseMotionEvent(const SDL_MouseMotionEvent& e) const
{
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
	//const int16_t tolerance = 6000;
	//const double x_modifier = ship->max_thrust.x / (std::numeric_limits<int16_t>::max() - tolerance);
	//const double y_modifier = ship->max_thrust.y / (std::numeric_limits<int16_t>::max() - tolerance);
	//if (!_controller)
	//	return;
	//int16_t left_x_raw = SDL_GameControllerGetAxis(_controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
	//int16_t left_y_raw = SDL_GameControllerGetAxis(_controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
	//int16_t left_x = util::getsign(left_x_raw)*std::max(0, (std::abs(left_x_raw) - tolerance));
	//int16_t left_y = util::getsign(left_y_raw)*std::max(0, (std::abs(left_y_raw) - tolerance));
	//if (std::abs(left_x) > 0)
	//{
	//	ship->state.current.thrust.x = left_x * x_modifier;
	//	this->turnShip(ship, left_x > 0 ? ShipDirection::right : ShipDirection::left);
	//}
	//if (std::abs(left_y) > 0)
	//{
	//	ship->state.current.thrust.y = left_y * y_modifier;
	//}
	//if (SDL_GameControllerGetButton(_controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X))
	//{
	//	ship->fire();
	//}
}

void InputSystem::impl::turnWithThrustConsideration(HOrient requested, HOrient& current, ThrustComponent& thrust) const
{
	if (requested == current)
		return;
	thrust.current.x = 0;	// one frame to turn without modifying thrust - allow for a turn without necessarily reducing existing momentum
	current = requested;
}