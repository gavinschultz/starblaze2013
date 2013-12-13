#pragma once
#include <SDL.h>

class Camera;

class Renderable
{
public:
	Renderable();
	~Renderable();
	virtual void draw(SDL_Renderer* sdl_renderer, const Camera& camera) = 0;
};