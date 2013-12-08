#include "stdafx.h"
#include <cmath>
#include <string>
#include <SDL_image.h>
#include "renderutil.h"
#include "program.h"

//int32_t renderutil::getScreenXForEntityByCameraAndDistance(double entity_x_at_zero, uint32_t entity_sprite_width, uint32_t world_width, const Camera& camera, double distance_factor)
//{
//	double camera_x_abs = camera.view_rect.x;
//
//	double camera_x_rel;
//	if (camera_x_abs > world_width / 2)
//		camera_x_rel = camera_x_abs - world_width;
//	else
//		camera_x_rel = camera_x_abs;
//
//	double entity_x_at_camera_x = entity_x_at_zero - (camera_x_rel * distance_factor);
//
//	if (entity_x_at_camera_x + entity_sprite_width > world_width * distance_factor)
//		entity_x_at_camera_x -= world_width * distance_factor;
//
//	return std::lround(entity_x_at_camera_x);
//}

SDL_Texture* renderutil::loadTextureFromFile(SDL_Renderer* sdl_renderer, std::string image_path, SDL_Rect* texture_rect)
{
	SDL_Surface* sdl_surface = IMG_Load(image_path.c_str());
	if (!sdl_surface)
	{
		program::exit(RetCode::sdl_error, { "Error loading file '", image_path, "': ", SDL_GetError() });
	}
	SDL_Texture* sdl_texture = SDL_CreateTextureFromSurface(sdl_renderer, sdl_surface);
	if (!sdl_texture)
	{
		program::exit(RetCode::sdl_error, { "Error creating texture from surface for image '", image_path, "': ", SDL_GetError() });
	}
	SDL_FreeSurface(sdl_surface);
	if (SDL_SetTextureBlendMode(sdl_texture, SDL_BLENDMODE_BLEND) < 0)
	{
		debug::console({ "Unable to set texture blend mode for image '", image_path, "': ", SDL_GetError() });
	}
	if (texture_rect != nullptr)
	{
		texture_rect->x = 0;
		texture_rect->y = 0;
		texture_rect->w = sdl_surface->w;
		texture_rect->h = sdl_surface->h;
	};
	return sdl_texture;
}

SDL_Rect renderutil::scaleRect(const SDL_Rect& rect, unsigned int scaling)
{
	return SDL_Rect{ rect.x, rect.y, rect.w * scaling, rect.h * scaling };
}