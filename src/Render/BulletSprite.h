#pragma once
#include "Sprite.h"

class Renderer;
class Camera;
class Bullet;

class BulletSprite : public Sprite
{
public:
	BulletSprite(Renderer* renderer);
	~BulletSprite();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const Bullet& bullet) const;
};