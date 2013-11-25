#include "stdafx.h"
#include "Renderer.h"
#include "Game.h"
#include "StationSprite.h"

StationSprite::StationSprite(Renderer* renderer, const Station& station) : Sprite(renderer->scaling)
{
	if (station.station_type == StationType::Fuel)
		_station_texture = renderer->sprite_loader.getSprite("fuel");
	else
		_station_texture = renderer->sprite_loader.getSprite("repair");
}

StationSprite::~StationSprite() {}

void StationSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera, const Station& station) const
{
	SDL_Rect station_rect = 
	{ 
		renderutil::getScreenXForEntityByCameraAndDistance(station.pos.x*_scaling, _station_texture.rect.w*_scaling, renderer->width, camera, 1.0), 
		game->ship_limits.h * _scaling - _station_texture.rect.h * _scaling,
		_station_texture.rect.w*_scaling,
		_station_texture.rect.h*_scaling
	};
	SDL_RenderCopy(sdl_renderer, _station_texture.texture, &_station_texture.rect, &station_rect);
}