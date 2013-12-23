#include "stdafx.h"
#include <glew.h>
#include "camera.h"
#include "playfield.h"
#include "renderdefs.h"
#include "component.h"
#include "mathutil.h"

void ZeroLineRender::render(SDL_Renderer* sdl_renderer, Window window, const Camera& camera) const
{
	auto playfield = db->getPlayField();
	if (!playfield)
		return;

	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	int zero_point_x = playfield->w - camera.getViewRect().x;
	SDL_RenderDrawLine(sdl_renderer, zero_point_x, 0, zero_point_x, window.h);
}