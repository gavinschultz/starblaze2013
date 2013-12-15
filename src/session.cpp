#include "stdafx.h"
#include "session.h"
#include "prefs.h"
#include "render\rendersystem.h"

bool session::is_paused = false;
bool session::quit = false;
int session::lives = 0;
int session::score = 0;
SessionState session::state = SessionState::title;

void session::handleKeyboardEvent(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
	case SDLK_q:
	case SDLK_ESCAPE:
		quit = true;
		return;
	case SDLK_F2:
		prefs::show_fps = !prefs::show_fps;
		prefs::show_debug = prefs::show_fps;
		prefs::show_grid = prefs::show_fps;
		break;
	case SDLK_F6:
		prefs::smooth_animation = !prefs::smooth_animation;
		break;
	case SDLK_F10:
		prefs::tmp_toggle = !prefs::tmp_toggle;
		break;
	case SDLK_RETURN:
		if (e.keysym.mod & KMOD_ALT)
			renderer->setFullscreen(!renderer->isFullscreen());
		break;
	}
}