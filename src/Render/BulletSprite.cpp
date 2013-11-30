#include "stdafx.h"
#include "Renderer.h"
#include "BulletSprite.h"
#include "Entity\Bullet.h"

BulletSprite::BulletSprite(Renderer* renderer) : Sprite(renderer->scaling) {}
BulletSprite::~BulletSprite() {}

void BulletSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera, const Bullet& bullet) const
{
	SDL_Rect bullet_rect = {
		renderutil::getScreenXForEntityByCameraAndDistance(bullet.alpha_pos.x * _scaling, bullet.bounding_box.w * _scaling, bullet.bounding_box.h * _scaling, camera, 1.0),
		std::lround(bullet.alpha_pos.y * _scaling - camera.view_rect.y),
		bullet.bounding_box.w * _scaling,
		bullet.bounding_box.h * _scaling
	};
	const SDL_Color& color = renderer->palette->colors[CoCoPaletteEnum::white];
	SDL_SetRenderDrawColor(sdl_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(sdl_renderer, &bullet_rect);
}