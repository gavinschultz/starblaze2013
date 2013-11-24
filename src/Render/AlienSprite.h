#pragma once
#include "Sprite.h"
#include "Renderer.h"
#include "Entity.h"

class AlienSprite : public Sprite
{
private:
	Alien* _alien;
	SDL_Texture* _alien_texture;
	SDL_Rect _alien_texture_rect;
public:
	AlienSprite(Renderer* renderer, Alien* alien);
	~AlienSprite();
	void render(SDL_Renderer* sdl_renderer, const Camera &camera);
};