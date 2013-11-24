#include "stdafx.h"
#include <cmath>
#include <string>
#include <SDL_image.h>
#include "RenderUtil.h"
#include "Debug.h"

int32_t renderutil::getScreenXForEntityByCameraAndDistance(double entity_x_at_zero, uint32_t entity_sprite_width, uint32_t world_width, const Camera& camera, double distance_factor)
{
	double camera_x_abs = camera.view_rect.x;

	double camera_x_rel;
	if (camera_x_abs > world_width / 2)
		camera_x_rel = camera_x_abs - world_width;
	else
		camera_x_rel = camera_x_abs;

	double entity_x_at_camera_x = entity_x_at_zero - (camera_x_rel * distance_factor);

	if (entity_x_at_camera_x + entity_sprite_width > world_width * distance_factor)
		entity_x_at_camera_x -= world_width * distance_factor;

	return std::lround(entity_x_at_camera_x);
}

SDL_Texture* renderutil::loadTextureFromFile(SDL_Renderer* sdl_renderer, std::string imagePath, SDL_Rect* texture_rect)
{
	SDL_Surface* sdlSurface = IMG_Load(imagePath.c_str());
	if (!sdlSurface)
	{
		console_debug({ SDL_GetError() });
		return nullptr;
	}
	SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(sdl_renderer, sdlSurface);
	SDL_FreeSurface(sdlSurface);
	SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
	if (texture_rect != nullptr)
	{
		texture_rect->x = 0;
		texture_rect->y = 0;
		texture_rect->w = sdlSurface->w;
		texture_rect->h = sdlSurface->h;
	};
	return sdlTexture;
}