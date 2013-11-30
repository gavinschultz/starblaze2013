#include "stdafx.h"
#include "Renderer.h"
#include "Util.h"
#include "Game.h"
#include "CoCoPalette.h"
#include "RadarSprite.h"
#include "Entity\World.h"
#include "Entity\Ship.h"
#include "Entity\Alien.h"
#include "Entity\Station.h"

RadarSprite::RadarSprite(Renderer* renderer) : Sprite(renderer->scaling)
{
	_radar_rect = {
		64 * (int32_t)renderer->scaling,
		156 * (int32_t)renderer->scaling,
		128 * (int32_t)renderer->scaling,
		36 * (int32_t)renderer->scaling
	};
	_radar_scaling_x = _radar_rect.w / (double)renderer->width;
	_radar_scaling_y = _radar_rect.h / (double)(world->ship_limits.h*renderer->scaling);
	int viewport_width_radar = _radar_rect.w / (renderer->width / renderer->window.w);
	int viewport_radar_left = (_radar_rect.w / 2) - viewport_width_radar / 2 - (_point_size * (int)_scaling) / 2;
	int viewport_radar_right = (_radar_rect.w / 2) + viewport_width_radar / 2 - (_point_size * (int)_scaling) / 2;
	_view_points = { {
		{ viewport_radar_left, 0 },
		{ viewport_radar_right, 0 },
		{ viewport_radar_left, _radar_rect.h - (_point_size * (int)renderer->scaling) },
		{ viewport_radar_right, _radar_rect.h - (_point_size * (int)renderer->scaling) }
	} };
	_radar_color = renderer->palette->colors[CoCoPaletteEnum::red];
	_point_color = renderer->palette->colors[CoCoPaletteEnum::yellow];
}

void RadarSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera) const
{
	int radar_left = calculateRadarLeft(camera);	// rendered world position at the left-most point of the radar

	SDL_SetRenderDrawColor(sdl_renderer, _radar_color.r, _radar_color.g, _radar_color.b, _radar_color.a);
	SDL_RenderFillRect(sdl_renderer, &_radar_rect);

	SDL_SetRenderDrawColor(sdl_renderer, _point_color.r, _point_color.g, _point_color.b, _radar_color.a);
	for (auto& vp : _view_points)
	{
		SDL_Rect point_rect = { _radar_rect.x + vp.x, _radar_rect.y + vp.y, _point_size * _scaling, _point_size * _scaling };
		SDL_RenderFillRect(sdl_renderer, &point_rect);
	}

	Ship* ship = game->entity_register.getShip();
	if (ship)
	{
		SDL_Rect point_rect = transformToRadarView(ship->alpha_pos.x, ship->alpha_pos.y, ship->bounding_box.w, ship->bounding_box.h, radar_left);
		SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(sdl_renderer, &point_rect);
	}

	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 190, 168);
	for (auto& alien : game->entity_register.getAliens())
	{
		SDL_Rect point_rect = transformToRadarView(alien->alpha_pos.x, alien->alpha_pos.y, alien->bounding_box.w, alien->bounding_box.h, radar_left);
		SDL_RenderFillRect(sdl_renderer, &point_rect);
	}

	Station* station = game->entity_register.getStation();
	if (station)
	{
		SDL_Rect point_rect = transformToRadarView(station->current_state.pos.x, station->current_state.pos.y, station->bounding_box.w, station->bounding_box.h, radar_left);
		SDL_SetRenderDrawColor(sdl_renderer, 56, 205, 212, 255);
		SDL_RenderFillRect(sdl_renderer, &point_rect);
	}
}

int RadarSprite::calculateRadarLeft(const Camera& camera) const
{
	int radar_middle = util::abswrap(camera.view_rect.x + (camera.view_rect.w / 2), renderer->width);
	int radar_left = util::abswrap(radar_middle - (int)(renderer->width / 2), renderer->width);
	return radar_left;
}

SDL_Rect RadarSprite::transformToRadarView(double entity_x, double entity_y, int entity_width, int entity_height, int radar_left) const
{
	// X
	int entity_x_midpoint = std::lround(entity_x * renderer->scaling) + ((entity_width * (int)renderer->scaling) / 2);
	int entity_x_midpoint_on_radar_unscaled = util::abswrap(entity_x_midpoint - radar_left, renderer->width);
	int entity_x_midpoint_on_radar_scaled = std::lround(entity_x_midpoint_on_radar_unscaled * _radar_scaling_x - (_point_size * _scaling) / 2);
	int radar_x = _radar_rect.x + entity_x_midpoint_on_radar_scaled;

	// Y
	//int entity_y_midpoint = std::lround(entity_y * renderer->scaling) + ((entity_height * (int)renderer->scaling));
	//int entity_y_midpoint_on_radar_unscaled = entity_y_midpoint - world->ship_limits.y*renderer->scaling;
	//int entity_y_midpoint_on_radar_scaled = entity_y_midpoint_on_radar_unscaled * _radar_scaling_y - (_point_size * _scaling) / 2;
	int entity_y_unscaled = std::lround(entity_y * renderer->scaling + entity_height * renderer->scaling);
	int entity_y_on_radar_scaled = std::lround(entity_y_unscaled * _radar_scaling_y - (_point_size * _scaling));
	int radar_y = _radar_rect.y + entity_y_on_radar_scaled;

	return SDL_Rect{ radar_x, radar_y, _point_size * renderer->scaling, _point_size * renderer->scaling };
}