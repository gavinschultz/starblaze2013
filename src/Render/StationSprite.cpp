#include "stdafx.h"
#include "Renderer.h"
#include "Game.h"
#include "StationSprite.h"

StationSprite::StationSprite(Renderer* renderer, Station* station)
{
	_scaling = renderer->scaling;
	_station_texture_rect = { 0, 0, 32, 16 };
	_station_texture = renderer->loadTextureFromFile("resources\\fuel_repair.tga", nullptr);
}

StationSprite::~StationSprite()
{

}

void StationSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera)
{
	auto station = game->entity_register.getStation();
	int station_x = renderutil::getScreenXForEntityByCameraAndDistance(station->pos.x*_scaling, _station_texture_rect.w*_scaling, renderer->width, camera, 1.0);
	int station_y = game->ship_limits.h * _scaling - _station_texture_rect.h * _scaling;
	SDL_Rect station_rect = { station_x, station_y, _station_texture_rect.w*_scaling, _station_texture_rect.h*_scaling };
	SDL_RenderCopy(sdl_renderer, _station_texture, &_station_texture_rect, &station_rect);
}