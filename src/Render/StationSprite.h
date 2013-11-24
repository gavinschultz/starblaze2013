#pragma once
#include "Sprite.h"
#include "Entity.h"
#include "Renderer.h"
#include <memory>

class StationSprite : public Sprite
{
private:
	SpriteTexture _station_texture;
	int station_type;	// 0 = fuel, 1 = repair
public:
	StationSprite(Renderer* renderer, Station* station);
	~StationSprite();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera);
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const Entity& entity);
};