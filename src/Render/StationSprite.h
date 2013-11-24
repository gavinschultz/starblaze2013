#pragma once
#include "Sprite.h"
#include "Entity.h"
#include "Renderer.h"
#include <memory>

class StationSprite : public Sprite
{
private:
	std::shared_ptr<Station> station;	//TODO: would rather not store reference here, should pass in at render time
	SDL_Texture* _station_texture;
	SDL_Rect _station_texture_rect;
	int station_type;	// 0 = fuel, 1 = repair
public:
	StationSprite(Renderer* renderer, Station* station);
	~StationSprite();
	void render(SDL_Renderer* sdlRenderer, const Camera& camera);
};