#include "stdafx.h"
#include "Renderer.h"
#include "Game.h"
#include "StationSprite.h"

StationSprite::StationSprite(Renderer* renderer, Station* station) : Sprite(renderer->scaling)
{
	if (station->station_type == 0)
		_station_texture = renderer->sprite_loader.getSprite("fuel");
	else
		_station_texture = renderer->sprite_loader.getSprite("repair");
	_station_texture.scaleRect(renderer->scaling);
}

StationSprite::~StationSprite() {}

void StationSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera)
{
	auto station = game->entity_register.getStation();
	this->render(sdl_renderer, camera, *station);
}

void StationSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera, const Entity& entity)
{
	auto& station = (Station&)entity;
	int station_x = renderutil::getScreenXForEntityByCameraAndDistance(station.pos.x*_scaling, _station_texture.srect.w, renderer->width, camera, 1.0);
	int station_y = game->ship_limits.h * _scaling - _station_texture.srect.h;
	SDL_Rect station_rect = { station_x, station_y, _station_texture.srect.w, _station_texture.srect.h };
	SDL_RenderCopy(sdl_renderer, _station_texture.texture, &_station_texture.rect, &station_rect);
}