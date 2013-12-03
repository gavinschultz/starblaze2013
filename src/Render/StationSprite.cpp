#include "stdafx.h"
#include "Renderer.h"
#include "Entity\World.h"
#include "StationSprite.h"
#include "Entity\Station.h"

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
		renderutil::getScreenXForEntityByCameraAndDistance(station.current_state.pos.x*_scaling, _station_texture.rect.w*_scaling, renderer->width, camera, 1.0), 
		world->ship_limits.h * _scaling - _station_texture.rect.h * _scaling,
		_station_texture.rect.w*_scaling,
		_station_texture.rect.h*_scaling
	};

	if (station.is_docked)
		SDL_SetTextureColorMod(_station_texture.texture, 0, 0, 0);
	else
		SDL_SetTextureColorMod(_station_texture.texture, 255, 255, 255);

	SDL_RenderCopy(sdl_renderer, _station_texture.texture, &_station_texture.rect, &station_rect);
}