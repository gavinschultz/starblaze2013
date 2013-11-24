#include "stdafx.h"
#include <SDL.h>
#include "Camera.h"

Camera::Camera(const SDL_Rect& view_rect, const SDL_Rect& focus_rect)
{
	this->view_rect = view_rect;
	this->focus_rect = focus_rect;
	this->focus_point = { 0, 0 };
}