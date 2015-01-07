#include "stdafx.h"
#include "rendersystem.h"
#include "cocopalette.h"
#include "renderdefs.h"
#include "textrender.h"
#include "mathutil.h"

class HUDRender::impl
{
public:
	SDL_Rect fuel_rect;
	SDL_Rect bullets_rect;
	SDL_Rect shields_rect;
	SDL_Rect radar_rect;
	std::array<SDL_Rect*, 4> gauge_rects = std::array<SDL_Rect*, 4>{ { &fuel_rect, &bullets_rect, &shields_rect, &radar_rect } };
	SDL_Color gauge_color;
	SDL_Color label_color;
};

HUDRender::HUDRender(const RenderSystem& renderer) : pi{ new impl() }
{
	pi->fuel_rect = { 16, 628, 32, 96 };
	pi->bullets_rect = { 80, 628, 32, 96 };
	pi->shields_rect = { 144, 628, 32, 96 };
	pi->radar_rect = { 208, 628, 32, 96 };
	pi->gauge_color = renderer.palette->colors[CoCoPaletteEnum::blue];
	pi->label_color = renderer.palette->colors[CoCoPaletteEnum::red];
}
HUDRender::~HUDRender() = default;
void HUDRender::render(SDL_Renderer* sdl_renderer, const PlayerComponent& player, const FireBulletsComponent& fire, unsigned int score, unsigned int lives, const TextRender& text_renderer) const
{
	// Fuel, bullets, shields, radar
	SDL_SetRenderDrawColor(sdl_renderer, pi->gauge_color.r, pi->gauge_color.g, pi->gauge_color.b, 255);
	SDL_RenderFillRects(sdl_renderer, *(pi->gauge_rects.data()), 4);

	float ammo_percent = (float)fire.currentAmmoCount() / (float)fire.maxAmmoCount();
	pi->bullets_rect.h = std::ceil(96 * ammo_percent);
	pi->bullets_rect.y = 724 - pi->bullets_rect.h;
	//debug::set("Ammo", std::to_string(fire.currentAmmoCount()));
	//debug::set("Ammo %", std::to_string(ammo_percent));
	//debug::set("T height", std::to_string(pi->bullets_rect.h));

	text_renderer.renderChar(sdl_renderer, 'F', { 8, 736 }, pi->label_color);
	text_renderer.renderChar(sdl_renderer, 'T', { 72, 736 }, pi->label_color);
	text_renderer.renderChar(sdl_renderer, 'S', { 136, 736 }, pi->label_color);
	text_renderer.renderChar(sdl_renderer, 'R', { 200, 736 }, pi->label_color);
}