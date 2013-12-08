#include "stdafx.h"
#include <array>
#include "CoCoPalette.h"
#include "HUDRend.h"
#include "Renderer.h"
#include "TextRenderer.h"

class HUDRend::impl
{
public:
	SDL_Rect fuel_rect;
	SDL_Rect bullets_rect;
	SDL_Rect shields_rect;
	SDL_Rect radar_rect;
	std::array<SDL_Rect*, 4> gauge_rects = std::array<SDL_Rect*, 4>{ { &fuel_rect, &bullets_rect, &shields_rect, &radar_rect } };
	SDL_Color gauge_color;
};

HUDRend::HUDRend(Renderer* renderer) : Sprite(renderer->scaling), pimpl{ new impl{} }
{
	pimpl->fuel_rect = { 4 * _scaling, 157 * _scaling, 8 * _scaling, 24 * _scaling };
	pimpl->bullets_rect = { 20 * _scaling, 157 * _scaling, 8 * _scaling, 24 * _scaling };
	pimpl->shields_rect = { 36 * _scaling, 157 * _scaling, 8 * _scaling, 24 * _scaling };
	pimpl->radar_rect = { 52 * _scaling, 157 * _scaling, 8 * _scaling, 24 * _scaling };
	pimpl->gauge_color = renderer->palette->colors[CoCoPaletteEnum::blue];
}

HUDRend::~HUDRend(){}

void HUDRend::render(SDL_Renderer* sdl_renderer, const TextRenderer& text_renderer, const Ship& ship, int score, int player_ships) const
{
	// Fuel, bullets, shields, radar
	SDL_SetRenderDrawColor(sdl_renderer, pimpl->gauge_color.r, pimpl->gauge_color.g, pimpl->gauge_color.b, 255);
	SDL_RenderFillRects(sdl_renderer, *(pimpl->gauge_rects.data()), 4);

	text_renderer.RenderChar(sdl_renderer, 'F', { 2 * (int32_t)_scaling, 184 * (int32_t)_scaling }, CoCoPaletteEnum::red);
	text_renderer.RenderChar(sdl_renderer, 'T', { 18 * (int32_t)_scaling, 184 * (int32_t)_scaling }, CoCoPaletteEnum::red);
	text_renderer.RenderChar(sdl_renderer, 'S', { 34 * (int32_t)_scaling, 184 * (int32_t)_scaling }, CoCoPaletteEnum::red);
	text_renderer.RenderChar(sdl_renderer, 'R', { 50 * (int32_t)_scaling, 184 * (int32_t)_scaling }, CoCoPaletteEnum::red);

	// Ships remaining

	// Score

}