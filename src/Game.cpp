#include "stdafx.h"
#include "game.h"

void Game::handleKeyboardEvent(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
	case SDLK_q:
	case SDLK_ESCAPE:
		this->quit = true;
		return;
	}
}