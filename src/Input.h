#pragma once
#include <SDL.h>
#include "Entity\Ship.h"

class Input
{
private:
	SDL_GameController* _controller = nullptr;
	void handleKeyboardEvent(const SDL_KeyboardEvent& e) const;
	void handleKeyboardState(Ship* ship) const;
	void handleMouseMotionEvent(const SDL_MouseMotionEvent& e, Ship* ship) const;
	void handleJoystickState(Ship* ship) const;

	bool turnShip(Ship* ship, ShipDirection requested_direction) const;
public:
	Input();
	~Input();
	void handleInput();
};