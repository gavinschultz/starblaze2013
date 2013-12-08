#pragma once
#include <memory>
#include <SDL.h>

class Game
{
public:
	void handleKeyboardEvent(const SDL_KeyboardEvent& e);
	bool quit{ false };
	bool is_paused{ false };
};

extern std::unique_ptr<Game> game;