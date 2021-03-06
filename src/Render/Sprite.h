#pragma once
#include <SDL.h>

class Camera;
class Entity;

class Sprite
{
protected:
	unsigned int _scaling;
public:
	Sprite();
	Sprite(unsigned int scaling);
	~Sprite();
	virtual void render(SDL_Renderer* sdl_renderer, const Camera& camera) {};
	virtual void render(SDL_Renderer* sdl_renderer, const Camera& camera, const Entity& entity){};
};