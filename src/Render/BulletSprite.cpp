#include "stdafx.h"
#include "Renderer.h"
#include "BulletSprite.h"
#include "Entity\Bullet.h"

BulletSprite::BulletSprite(Renderer* renderer) : Sprite(renderer->scaling) {}
BulletSprite::~BulletSprite() {}

void BulletSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera, const Bullet& bullet) const
{
	if (!bullet.is_active)
		return;

	SDL_Rect bullet_rect = {
		renderutil::getScreenXForEntityByCameraAndDistance(bullet.state.interpolated.x * _scaling, bullet.box.w * _scaling, renderer->width, camera, 1.0),
		std::lround(bullet.state.interpolated.y * _scaling - camera.view_rect.y),
		bullet.box.w * _scaling,
		bullet.box.h * _scaling
	};
	const SDL_Color& color = renderer->palette->colors[CoCoPaletteEnum::red];
	SDL_SetRenderDrawColor(sdl_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(sdl_renderer, &bullet_rect);
}