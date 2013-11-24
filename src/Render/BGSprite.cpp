#include "stdafx.h"
#include <array>
#include "CoCoPalette.h"
#include "Renderer.h"
#include "BGSprite.h"

BGSprite::BGSprite(Renderer* renderer, World* world) : Sprite()
{
	_scaling = renderer->scaling;
	_world = std::shared_ptr<World>(world);
	_sky_rect = { 0, 0, renderer->window.w, 96 * _scaling };
	_sky_color = renderer->coco_palette.getColor(blue);
	_ground_rect = { 0, _sky_rect.h, renderer->window.w, 48 * _scaling };
	_ground_color = renderer->coco_palette.getColor(green);
	_hills_texture = renderer->loadTextureFromFile("resources\\hills.tga", nullptr);
	_hills_texture_rect = { 0, 0, 16, 4 };
	_hills_rect = { 0, _ground_rect.y, renderer->window.w, 28 * _scaling };
	_bg_rect = { 0, 0, renderer->window.w, renderer->window.h };
	_bg_color = renderer->coco_palette.getColor(green);
}

BGSprite::~BGSprite()
{

}

void BGSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera)
{
	const std::array<int, 4> y_channel_coords = { { 0 - _hills_texture_rect.h * _scaling, 9 * _scaling, 17 * _scaling, 25 * _scaling } };
	SDL_SetRenderDrawColor(sdl_renderer, _bg_color.r, _bg_color.g, _bg_color.b, _bg_color.a);
	SDL_RenderFillRect(sdl_renderer, &_bg_rect);
	SDL_SetRenderDrawColor(sdl_renderer, _sky_color.r, _sky_color.g, _sky_color.b, _sky_color.a);
	SDL_RenderFillRect(sdl_renderer, &_sky_rect);
	SDL_SetRenderDrawColor(sdl_renderer, _ground_color.r, _ground_color.g, _ground_color.b, _ground_color.a);
	SDL_RenderFillRect(sdl_renderer, &_ground_rect);

	for (auto hill : _world->hills)
	{
		_hills_texture_rect.y = _hills_texture_rect.h * hill.type;
		_hills_texture_rect.x = (hill.y_channel == 0) ? 16 : 0;	// Blue version for ground, green version for horizon
		int32_t entity_x_at_camera_x = renderutil::getScreenXForEntityByCameraAndDistance(hill.x*_scaling, _hills_texture_rect.w*_scaling, (int)world->w*_scaling, camera, hill.distance_factor);
		SDL_Rect hill_rect = { entity_x_at_camera_x, _hills_rect.y + y_channel_coords[hill.y_channel], _hills_texture_rect.w * _scaling, _hills_texture_rect.h * _scaling };
		SDL_RenderCopy(sdl_renderer, _hills_texture, &_hills_texture_rect, &hill_rect);
	}
}