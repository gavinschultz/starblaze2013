#include "stdafx.h"
#include <SDL.h>
#include "Game.h"
#include "Render.h"
#include "Input.h"
#include "Util.h"
#include <limits>

Input::Input()
{
	SDL_GameController* controller = nullptr;
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if (SDL_IsGameController(i))
		{
			controller = SDL_GameControllerOpen(i);
			if (!controller)
			{
				console_debug({ "Could not open controller: ", SDL_GetError() });
				continue;
			}
			console_debug({ "Controller detected: ", SDL_GameControllerName(controller) });
			console_debug({ "Controller mapping: ", SDL_GameControllerMapping(controller) });
			_controller = controller;
			break;
		}
	}
}

Input::~Input() {}

void Input::handleInput()
{
	Ship* ship = game->entity_register.getShip();
	SDL_Event pevent;
	while (SDL_PollEvent(&pevent))
	{
		if (game->is_paused && pevent.type != SDL_KEYDOWN)
			continue;

		switch (pevent.type)
		{
		case SDL_KEYDOWN:
			this->handleKeyboardEvent(pevent.key);
			break;
		case SDL_MOUSEMOTION:
			this->handleMouseMotionEvent(pevent.motion, ship);
			break;
		default:
			break;
		}
	}

	if (game->is_paused)
		return;

	this->handleJoystickState(ship);
	this->handleKeyboardState(ship);
}

void Input::handleKeyboardEvent(const SDL_KeyboardEvent& e) const
{
	switch (e.keysym.sym)
	{
	case SDLK_ESCAPE:
	case SDLK_q:
		game->quit = true;
		return;
	case SDLK_F6:
		game->mouse_sensitivity = std::max(2.0f, game->mouse_sensitivity - 0.5f);
		break;
	case SDLK_F7:
		game->mouse_sensitivity = std::min(6.0f, game->mouse_sensitivity + 0.5f);
		break;
	case SDLK_F10:
		renderer->toggleMotionHistory(!renderer->is_motionhistory_visible);
		break;
	case SDLK_F11:
		renderer->toggleGrid(!renderer->is_grid_visible);
		break;
	case SDLK_p:
		game->togglePause(!game->is_paused);
		break;
	case SDLK_f:
		game->advanceFrameByFrame();
		break;
	case SDLK_RETURN:
		if (e.keysym.mod & KMOD_ALT)
			renderer->toggleFullscreen(!renderer->is_fullscreen);
		break;
	}
}

void Input::handleKeyboardState(Ship* ship) const
{
	auto* keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_UP])
		ship->current_state.thrust.y = -ship->max_thrust.y;
	if (keystate[SDL_SCANCODE_DOWN])
		ship->current_state.thrust.y = ship->max_thrust.y;
	if (keystate[SDL_SCANCODE_LEFT])
	{
		ship->current_state.thrust.x = -ship->max_thrust.x;
		this->turnShip(ship, ShipDirection::left);
	}
	if (keystate[SDL_SCANCODE_RIGHT])
	{
		ship->current_state.thrust.x = ship->max_thrust.x;
		this->turnShip(ship, ShipDirection::right);
	}
}

void Input::handleMouseMotionEvent(const SDL_MouseMotionEvent& e, Ship* ship) const
{
	// Ship control via the mouse is kinda shitty, only because I haven't found a way to easily get perpetual thrust in a single direction
	// Needs some experimentation - probably should try non-relative coordinates next
	if (!game->is_paused)
	{
		if (std::abs(e.xrel) > 2)
			ship->current_state.thrust.x = util::getsign(e.xrel) * ship->max_thrust.x;
		else
			ship->current_state.thrust.x = e.xrel * game->mouse_sensitivity * 16.0;
		ship->current_state.thrust.y = e.yrel * game->mouse_sensitivity;
		if (e.xrel > 0)
			this->turnShip(ship, ShipDirection::right);
		else if (e.xrel < 0)
			this->turnShip(ship, ShipDirection::left);
	}
}

void Input::handleJoystickState(Ship* ship) const
{
	const int16_t tolerance = 6000;
	const double x_modifier = ship->max_thrust.x / (std::numeric_limits<int16_t>::max() - tolerance);
	const double y_modifier = ship->max_thrust.y / (std::numeric_limits<int16_t>::max() - tolerance);
	if (!_controller)
		return;
	int16_t left_x_raw = SDL_GameControllerGetAxis(_controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
	int16_t left_y_raw = SDL_GameControllerGetAxis(_controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
	int16_t left_x = util::getsign(left_x_raw)*std::max(0, (std::abs(left_x_raw) - tolerance));
	int16_t left_y = util::getsign(left_y_raw)*std::max(0, (std::abs(left_y_raw) - tolerance));
	if (std::abs(left_x) > 0)
	{
		ship->current_state.thrust.x = left_x * x_modifier;
		this->turnShip(ship, left_x > 0 ? ShipDirection::right : ShipDirection::left);
	}
	if (std::abs(left_y) > 0)
	{
		ship->current_state.thrust.y = left_y * y_modifier;
	}
}

bool Input::turnShip(Ship* ship, ShipDirection requested_direction) const
{
	bool turned = false;
	if (ship->direction != requested_direction)
	{
		ship->current_state.thrust.x = 0;	// one frame to turn without modifying thrust
		turned = true;
	}
	ship->direction = requested_direction;
	return turned;
}