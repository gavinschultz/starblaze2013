#include "stdafx.h"
#include <array>
#include "rendersystem.h"
#include "camera.h"
#include "cocopalette.h"
#include "component.h"
#include "renderdefs.h"
#include "playfield.h"

class RadarRender::impl
{
public:
	Vector2D radar_scaling;
	SDL_Rect radar_rect;
	SDL_Color radar_color;
	SDL_Color point_color;
	std::array<Point2Di, 4> view_points;
	const int point_size{ 8 };
	int calculateRadarLeft(const Camera& camera, const PlayField& playfield) const
	{
		int radar_middle = std::lround(playfield.getAbsolutePosX(camera.getViewRect().x + (camera.getViewRect().w / 2)));
		int radar_left = std::lround(playfield.getAbsolutePosX(radar_middle - (int)(playfield.w / 2)));
		return radar_left;
	}
	SDL_Rect transformToRadarView(double entity_x, double entity_y, double entity_width, double entity_height, int radar_left, const PlayField& playfield) const
	{
		// X
		int entity_x_midpoint = std::lround(entity_x + (entity_width / 2));
		int entity_x_midpoint_on_radar_unscaled = std::lround(playfield.getAbsolutePosX(entity_x_midpoint - radar_left));
		int entity_x_midpoint_on_radar_scaled = std::lround(entity_x_midpoint_on_radar_unscaled * radar_scaling.x - (point_size / 2));
		int radar_x = radar_rect.x + entity_x_midpoint_on_radar_scaled;

		// Y
		int entity_y_unscaled = std::lround(entity_y + entity_height);
		int entity_y_on_radar_scaled = std::lround(entity_y_unscaled * radar_scaling.y - point_size);
		int radar_y = radar_rect.y + entity_y_on_radar_scaled;

		return SDL_Rect{ radar_x, radar_y, point_size, point_size };
	}
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent* state, const PhysicalComponent* physical) const
	{
		PlayField* playfield = db->getPlayField();
		int radar_left = calculateRadarLeft(camera, *playfield);	// rendered world position at the left-most point of the radar

		SDL_SetRenderDrawColor(sdl_renderer, point_color.r, point_color.g, point_color.b, radar_color.a);
		for (auto& vp : view_points)
		{
			SDL_Rect point_rect = { radar_rect.x + vp.x, radar_rect.y + vp.y, point_size, point_size };
			SDL_RenderFillRect(sdl_renderer, &point_rect);
		}
		
		SDL_Rect point_rect = transformToRadarView(state->interpolated.x, state->interpolated.y, physical->box.w, physical->box.h, radar_left, *playfield);
		SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(sdl_renderer, &point_rect);
	}
};

RadarRender::RadarRender(const RenderSystem& renderer) : pi{ new impl() }
{
	PlayField* playfield = db->getPlayField();
	pi->radar_color = renderer.palette->colors[CoCoPaletteEnum::red];
	pi->point_color = renderer.palette->colors[CoCoPaletteEnum::yellow];

	pi->radar_rect = { 256, 624, renderer.getWindow().w - (2*256), 144 };
	pi->radar_scaling.x = pi->radar_rect.w / (double)playfield->w;
	pi->radar_scaling.y = pi->radar_rect.h / (double)(playfield->boundaries.h);
	int viewport_width_radar = pi->radar_rect.w / ((int)playfield->w / renderer.getWindow().w);
	int viewport_radar_left = (pi->radar_rect.w / 2) - viewport_width_radar / 2 - (pi->point_size / 2);
	int viewport_radar_right = (pi->radar_rect.w / 2) + viewport_width_radar / 2 - (pi->point_size / 2);
	pi->view_points = { {
		{ viewport_radar_left, 0 },
		{ viewport_radar_right, 0 },
		{ viewport_radar_left, pi->radar_rect.h - pi->point_size },
		{ viewport_radar_right, pi->radar_rect.h - pi->point_size }
	} };
}
RadarRender::~RadarRender() = default;

void RadarRender::renderBox(SDL_Renderer* sdl_renderer)
{
	SDL_SetRenderDrawColor(sdl_renderer, pi->radar_color.r, pi->radar_color.g, pi->radar_color.b, pi->radar_color.a);
	SDL_RenderFillRect(sdl_renderer, &pi->radar_rect);
}
void RadarRender::render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent* state, const PhysicalComponent* physical) const
{
	pi->render(sdl_renderer, camera, state, physical);
}