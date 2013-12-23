#include "stdafx.h"
#include <vector>
#include <glew.h>
#include "renderdefs.h"
#include "renderutil.h"
#include "component.h"
#include "playfield.h"

void CollisionBoxRender::render(SDL_Renderer* sdl_renderer, const Camera& camera, const CollisionComponent& collision) const
{
	auto playfield = db->getPlayField();
	if (!playfield)
		return;

	SDL_Rect transformed_outer_rect = SDL_Rect{
		renderutil::getScreenXForEntityByCameraAndDistance(collision.outer_box.x, collision.outer_box.w, playfield->w, camera, 1.0),
		collision.outer_box.y,
		collision.outer_box.w,
		collision.outer_box.h
	};
	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdl_renderer, 255, 45, 45, 128);
	SDL_RenderDrawRect(sdl_renderer, &transformed_outer_rect);

	std::vector<SDL_Rect> transformed_rects;
	for (auto& collision_box : collision.inner_boxes)
	{
		SDL_Rect transformed_rect = SDL_Rect{
		renderutil::getScreenXForEntityByCameraAndDistance(collision_box.x, collision_box.w, playfield->w, camera, 1.0),
		collision_box.y,
		collision_box.w,
		collision_box.h };
		transformed_rects.push_back(transformed_rect);
	}
	glLineWidth(1.0f);
	SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRects(sdl_renderer, transformed_rects.data(), transformed_rects.size());
}