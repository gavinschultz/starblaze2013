#include "stdafx.h"
#include <glew.h>
#include "phys.h"
#include "renderdefs.h"

void GridRender::render(SDL_Renderer* sdl_renderer, Window window, Rect playfield_rect) const
{
	glPointSize(1.0f);
	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	for (uint32_t x = 0; x <= window.w; x += 8)
	{
		int y1 = 8;
		int line_height = 0;
		if ((x % 64) < 32)
			line_height = 32 - (x % 32);
		else if ((x % 64) == 32)
			line_height = 16;
		else
			line_height = x % 32;
		SDL_RenderDrawLine(sdl_renderer, x, y1, x, y1 + line_height);
	}

	glLineWidth(1.0f);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x0101);
	for (uint32_t x = 0; x < window.w; x += std::lround((double)(window.w) / 8))
	{
		SDL_RenderDrawLine(sdl_renderer, x, 0, x, window.h);
	}
	SDL_RenderDrawLine(sdl_renderer, window.w - 1, 0, window.w - 1, window.h);

	for (uint32_t y = 0; y <= window.h; y += std::lround((double)(window.h) / 4))
	{
		SDL_RenderDrawLine(sdl_renderer, 0, y, window.w - 1, y);
	}
	SDL_RenderDrawLine(sdl_renderer, 0, window.h - 1, window.w, window.h - 1);
	
	SDL_SetRenderDrawColor(sdl_renderer, 64, 64, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(sdl_renderer, (int)playfield_rect.x, 0, (int)playfield_rect.x, window.h);
	SDL_RenderDrawLine(sdl_renderer, (int)(playfield_rect.x + playfield_rect.w), 0, (int)(playfield_rect.x + playfield_rect.w), window.h);
	glDisable(GL_LINE_STIPPLE);
}