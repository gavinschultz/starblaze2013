#pragma once
#include <SDL.h>
#include "Camera.h"
#include "Entity.h"

class Sprite
{
protected:
	unsigned int _scaling;
public:
	Sprite();
	Sprite(unsigned int scaling);
	~Sprite();
	virtual void render(SDL_Renderer* sdlRenderer, const Camera& camera) {};
	virtual void render(SDL_Renderer* sdlRenderer, const Camera& camera, const Entity& entity){};
};