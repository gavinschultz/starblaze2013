#pragma once
#include "Sprite.h"
#include "Entity.h"
#include "Renderer.h"
#include <memory>

class StationSprite : public Sprite
{
private:
	SpriteTexture _station_texture;
public:
	StationSprite(Renderer* renderer, const Station& entity);
	~StationSprite();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const Station& station) const;
};