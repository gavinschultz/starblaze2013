#pragma once
#include <memory>
#include <SDL.h>

enum SessionState
{
	title,
	play,
	map,
	gameover
};

namespace session
{
	void handleKeyboardEvent(const SDL_KeyboardEvent& e);
	extern bool quit;
	extern bool paused;
	extern bool frame_by_frame;
	extern int score;
	extern int lives;
	extern SessionState state;
};