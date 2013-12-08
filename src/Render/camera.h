#pragma once
#include <SDL.h>
#include "Phys.h"

class Camera
{
public:
	Camera(const SDL_Rect& window_rect, const SDL_Rect& focus_rect);
	SDL_Rect view_rect;
	SDL_Rect focus_rect;
	SDL_Rect prev_focus_rect;
	Point2Di focus_point;
	int focus_loop_count{ 0 };
	int prev_focus_loop_count{ 0 };
	Vector2Di focus_point_vel;
	Point2Di prev_focus_point;
};