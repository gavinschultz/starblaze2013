#pragma once
#include "Sprite.h"
#include "Renderer.h"
#include <array>
#include <SDL.h>

class RadarSprite : public Sprite
{
private:
	const int _point_size{ 2 };
	double _radar_scaling_x;
	double _radar_scaling_y;
	SDL_Rect _radar_rect;
	std::array<Point2Di, 4> _view_points;
	SDL_Color _radar_color;
	SDL_Color _point_color;
	int _radar_left;	// rendered world position at the left-most point of the radar
	int calculateRadarLeft(const Camera& camera);
	SDL_Rect transformToRadarView(double entity_x, double entity_y, int entity_width, int entity_height);
public:
	RadarSprite(Renderer* renderer);
	void render(SDL_Renderer* sdlRenderer, const Camera& camera);
};