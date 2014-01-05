#include "stdafx.h"
#include <vector>
#include <glew.h>
#include "renderdefs.h"

void MotionHistoryRender::render(SDL_Renderer* sdl_renderer, Window window, const std::vector<Vector2D>& points, Vector2D thresholds, unsigned int current_index) const
{
	const int initial_y = (window.h / 2);
	const int threshold_line_offset = 75;
	const int axis_line_offset = 100;

	glLineWidth(1.0f);

	// background
	SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 176);
	auto bgrect = SDL_Rect{ 0, initial_y - axis_line_offset - threshold_line_offset - 50, window.w, (axis_line_offset + threshold_line_offset + 50) * 2 };
	SDL_RenderFillRect(sdl_renderer, &bgrect);

	// base lines
	SDL_SetRenderDrawColor(sdl_renderer, 128, 128, 128, 255);
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y - axis_line_offset, window.w, initial_y - axis_line_offset);
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y + axis_line_offset, window.w, initial_y + axis_line_offset);

	// threshold lines
	SDL_SetRenderDrawColor(sdl_renderer, 64, 64, 64, 255);
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y - axis_line_offset - threshold_line_offset, window.w, initial_y - axis_line_offset - threshold_line_offset); // X motion - upper threshold
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y + axis_line_offset - threshold_line_offset, window.w, initial_y + axis_line_offset - threshold_line_offset); // X motion - lower threshold
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y - axis_line_offset + threshold_line_offset, window.w, initial_y - axis_line_offset + threshold_line_offset); // Y motion - upper threshold
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y + axis_line_offset + threshold_line_offset, window.w, initial_y + axis_line_offset + threshold_line_offset); // Y motion - lower threshold

	// points
	glLineWidth(2.0f);
	Vector2Di render_point_y{ 0, 0 };
	Vector2Di prev_render_point_y;
	for (unsigned int i = 0; i < points.size(); i++)
	{
		Vector2D p = points[i];
		glPointSize(1.0f);

		int scaled_xmotion = std::lround(((float)threshold_line_offset / thresholds.x) * p.x); // X motion
		int scaled_ymotion = std::lround(((float)threshold_line_offset / thresholds.y) * p.y); // Y motion

		if (i > 0)
		{
			prev_render_point_y = render_point_y;
			render_point_y = { initial_y - axis_line_offset - scaled_xmotion, initial_y + axis_line_offset + scaled_ymotion };
			Vector2D prev_p = points[i - 1];

			int r, g, b;
			int frame_age = (int)current_index - (int)i;
			if (frame_age < 0)
				frame_age += points.size();
			b = 255 - std::min(frame_age * 2, 255);
			g = 155 + std::min(frame_age * 2, 255 - 155);
			r = 64 + std::min(frame_age, 255 - 64);

			SDL_SetRenderDrawColor(sdl_renderer, r, g, b, 255);
			SDL_RenderDrawLine(sdl_renderer, i - 1, prev_render_point_y.x, i, render_point_y.x);
			if (std::abs(scaled_xmotion) > threshold_line_offset)
			{
				SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(sdl_renderer, i, initial_y - axis_line_offset - (threshold_line_offset * std::copysign(1.0, scaled_xmotion)));
			}

			SDL_SetRenderDrawColor(sdl_renderer, r, g, b, 255);
			SDL_RenderDrawLine(sdl_renderer, i - 1, prev_render_point_y.y, i, render_point_y.y);
			if (std::abs(scaled_ymotion) > threshold_line_offset)
			{
				SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(sdl_renderer, i, initial_y + axis_line_offset + (threshold_line_offset * std::copysign(1.0, scaled_ymotion)));
			}
		}
	}
}