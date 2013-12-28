#include "stdafx.h"
#include "rendersystem.h"
#include "renderdefs.h"
#include "renderutil.h"
#include "playfield.h"

class BulletRender::impl
{
public:
	BulletRender::impl(SDL_Color& color) : color(color) {}
	SDL_Color& color;
};

BulletRender::BulletRender(const RenderSystem& renderer) : pi{ new impl(renderer.palette->colors[CoCoPaletteEnum::red]) } {}
BulletRender::~BulletRender() = default;

void BulletRender::render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent& state, PhysicalComponent& physical) const
{
	auto playfield = db->getPlayField();
	SDL_Rect bullet_rect = {
		renderutil::getScreenXForEntityByCameraAndDistance(state.interpolated.x, physical.box.w, playfield->w, camera, 1.0),
		std::lround(state.interpolated.y - camera.getViewRect().y),
		physical.box.w,
		physical.box.h
	};
	SDL_SetRenderDrawColor(sdl_renderer, pi->color.r, pi->color.g, pi->color.b, 255);
	SDL_RenderFillRect(sdl_renderer, &bullet_rect);
}