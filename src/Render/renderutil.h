#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Camera.h"

namespace renderutil
{
	int32_t getScreenXForEntityByCameraAndDistance(float entity_x_at_zero, unsigned int entity_sprite_width, unsigned int world_width, const Camera& camera, float distance_factor);
	SDL_Texture* loadTextureFromFile(SDL_Renderer* sdl_renderer, std::string imagePath, SDL_Rect* texture_rect);
	void loadGlTextureFromFile(SDL_Renderer* sdl_renderer, std::string imagePath, SDL_Rect* texture_rect);
	SDL_Rect scaleRect(const SDL_Rect& rect, unsigned int scaling);
	bool hasDimensions(SDL_Rect rect);
}

namespace render
{
	// Render text using the font loaded by bmfont::load()
	void renderSystemText_Bitmap(SDL_Renderer* sdl_renderer, const std::string& text, unsigned int x, unsigned int y);

	// This method uses the SDL_TTF library to display text. This is a bit more expensive than renderSystemText_Bitmap, though probably not to 
	// any significant degree in the context of the program
	void renderSystemText_TTF(SDL_Renderer* sdl_renderer, TTF_Font* font, const std::string& text, unsigned int x, unsigned int y);
}