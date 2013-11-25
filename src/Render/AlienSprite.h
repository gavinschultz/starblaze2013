#pragma once
#include "Sprite.h"
#include "Entity\Alien.h"

class Camera;
class Renderer;

class AlienSprite : public Sprite
{
private:
	SpriteTexture _alien_texture;
	Alien* _alien;
public:
	AlienSprite(Renderer* renderer, Alien* alien);
	~AlienSprite();
	void render(SDL_Renderer* sdl_renderer, const Camera &camera);
};