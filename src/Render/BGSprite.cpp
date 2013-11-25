#include "stdafx.h"
#include <array>
#include "CoCoPalette.h"
#include "Renderer.h"
#include "BGSprite.h"

BGSprite::BGSprite(Renderer* renderer) : Sprite(renderer->scaling)
{
	_sky_rect = { 0, 0, renderer->window.w, SKY_HEIGHT * _scaling };
	_ground_rect = { 0, _sky_rect.h, renderer->window.w, GROUND_HEIGHT * _scaling };
	_bg_rect = { 0, 0, renderer->window.w, renderer->window.h };
	_sky_color = renderer->coco_palette.getColor(blue);
	_ground_color = renderer->coco_palette.getColor(green);
	_bg_color = renderer->coco_palette.getColor(green);

	_hills_texture = renderer->sprite_loader.getSprite("hills");
}

BGSprite::~BGSprite()
{

}

void BGSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera, const World& world) const
{
	const std::array<int, 4> y_channel_coords = { { 0 - HILL_HEIGHT * _scaling, 9 * _scaling, 17 * _scaling, 25 * _scaling } };
	SDL_SetRenderDrawColor(sdl_renderer, _bg_color.r, _bg_color.g, _bg_color.b, _bg_color.a);
	SDL_RenderFillRect(sdl_renderer, &_bg_rect);
	SDL_SetRenderDrawColor(sdl_renderer, _sky_color.r, _sky_color.g, _sky_color.b, _sky_color.a);
	SDL_RenderFillRect(sdl_renderer, &_sky_rect);
	SDL_SetRenderDrawColor(sdl_renderer, _ground_color.r, _ground_color.g, _ground_color.b, _ground_color.a);
	SDL_RenderFillRect(sdl_renderer, &_ground_rect);

	SDL_Rect hill_texture_rect = _hills_texture.rect;
	hill_texture_rect.w = HILL_WIDTH;
	hill_texture_rect.h = HILL_HEIGHT;
	for (auto hill : world.hills)
	{
		hill_texture_rect.y = _hills_texture.rect.y + HILL_HEIGHT * hill.type;
		hill_texture_rect.x = _hills_texture.rect.x + ((hill.y_channel == 0) ? HILL_WIDTH : 0);	// Blue version for ground (>0), green version for horizon (0)
		int32_t entity_x_at_camera_x = renderutil::getScreenXForEntityByCameraAndDistance(hill.x*_scaling, hill_texture_rect.w*_scaling, renderer->width, camera, hill.distance_factor);
		SDL_Rect hill_rect = { 
			entity_x_at_camera_x, 
			SKY_HEIGHT * _scaling + y_channel_coords[hill.y_channel], 
			HILL_WIDTH * _scaling, 
			HILL_HEIGHT * _scaling 
		};
		SDL_RenderCopy(sdl_renderer, _hills_texture.texture, &hill_texture_rect, &hill_rect);
	}
}