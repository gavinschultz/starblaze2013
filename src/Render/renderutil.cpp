#include "stdafx.h"
#include <cmath>
#include <string>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "renderutil.h"
#include "program.h"
#include "bmfontloader.h"

#include "prefs.h"

int32_t renderutil::getScreenXForEntityByCameraAndDistance(double entity_x_at_zero, unsigned int entity_sprite_width, unsigned int world_width, const Camera& camera, double distance_factor)
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

void render::renderSystemText_Bitmap(SDL_Renderer* sdl_renderer, const std::string& text, unsigned int x, unsigned int y)
{
	if (prefs::tmp_toggle)
	{
		auto font_map = *bmfont::getMap();
		auto bmfile = *bmfont::getDefinition();
		for (auto c : text)
		{
			auto font_texture = font_map.at(c);
			auto chars_def = bmfile.chars.at(c);
			SDL_Rect render_rect = { x + chars_def.xoffset, y + chars_def.yoffset, font_texture.rect.w, font_texture.rect.h };
			SDL_RenderCopy(sdl_renderer, font_texture.texture, &font_texture.rect, &render_rect);
			x += chars_def.xadvance;
		}
	}
	else
	{
		 auto font_map = bmfont::getMap();
		 auto bmfile = bmfont::getDefinition();
		for (auto c : text)
		{
			auto font_texture = font_map->at(c);
			SDL_Rect render_rect = { x + bmfile->chars.at(c).xoffset, y + bmfile->chars.at(c).yoffset, font_texture.rect.w, font_texture.rect.h };
			SDL_RenderCopy(sdl_renderer, font_texture.texture, &font_texture.rect, &render_rect);
			x += bmfile->chars.at(c).xadvance;
		}
	}

	/*
	 * Something I'd like to look into later: the below version of this function, which omits the reference (&) on font_map and bmfile, and therefore
	 * invokes a copy.
	 *   a) technically runs faster (despite running more assembly code) and
	 *   b) reduces the frame rate to 20 or 30fps, suggesting VSYNC misses
	 * Only happens in debug mode though. 
	 * Something to do with cache coherency? Can't tell why it's happening from the disassembly, would be interesting to find out what's going on.
	 * with a deep profile.
	 *

	 auto font_map = *bmfont::getMap();
	 auto bmfile = *bmfont::getDefinition();
	 for (auto c : text)
	 {
		 auto font_texture = font_map.at(c);
		 auto chars_def = bmfile.chars.at(c);
		 SDL_Rect render_rect = { x + chars_def.xoffset, y + chars_def.yoffset, font_texture.rect.w, font_texture.rect.h };
		 SDL_RenderCopy(sdl_renderer, font_texture.texture, &font_texture.rect, &render_rect);
		 x += chars_def.xadvance;
	 }
	*/
}

void render::renderSystemText_TTF(SDL_Renderer* sdl_renderer, TTF_Font* font, const std::string& text, unsigned int x, unsigned int y)
{
	SDL_Color text_color = { 255, 255, 255 };
	SDL_Surface* text_surface = TTF_RenderText_Blended(font, text.c_str(), text_color);
	if (!text_surface)
	{
		debug::console({ "Unable to initialize font (mostly for debugging): ", SDL_GetError() });
		return;
	}
	SDL_Rect text_texture_rect = { 0, 0, text_surface->w, text_surface->h };
	SDL_Rect text_rect = { x, y, text_surface->w, text_surface->h };
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(sdl_renderer, text_surface);
	if (!text_texture)
	{
		debug::console({ "Unable to create texture from surface: ", SDL_GetError() });
		return;
	}
	SDL_FreeSurface(text_surface);
	if (SDL_RenderCopy(sdl_renderer, text_texture, &text_texture_rect, &text_rect) < 0)
	{
		debug::console({ "Unable to render text texture: ", SDL_GetError() });
		return;
	}
	SDL_DestroyTexture(text_texture);
}