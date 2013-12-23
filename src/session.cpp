#include "stdafx.h"
#include "session.h"
#include "prefs.h"
#include "render\rendersystem.h"

bool session::paused = false;
bool session::frame_by_frame = false;
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
		prefs::show_collision_boxes = prefs::show_fps;
		break;
	case SDLK_F3:
		prefs::show_motionhistory = !prefs::show_motionhistory;
		break;
	case SDLK_F6:
		prefs::smooth_animation = !prefs::smooth_animation;
		break;
	case SDLK_F8:
		prefs::vsync = !prefs::vsync;
		renderer->init();
		break;
	case SDLK_f:
		session::paused = true;
		session::frame_by_frame = true;
		break;
	case SDLK_p:
		session::paused = !session::paused;
		if (!session::paused)
			session::frame_by_frame = false;
		break;
	case SDLK_RETURN:
		if (e.keysym.mod & KMOD_ALT)
			renderer->setFullscreen(!renderer->isFullscreen());
		break;
	}
}