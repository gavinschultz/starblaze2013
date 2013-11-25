#include "stdafx.h"
#include "Renderer.h"
#include "AlienSprite.h"

AlienSprite::AlienSprite(Renderer* renderer, Alien* alien) : Sprite(renderer->scaling)
{
	this->_alien = alien;
	this->_alien_texture = renderer->sprite_loader.getSprite("alien1");
}

void AlienSprite::render(SDL_Renderer* sdl_renderer, const Camera &camera)
{
	SDL_Rect dstrect = 
	{
		renderutil::getScreenXForEntityByCameraAndDistance(_alien->alpha_pos.x*_scaling, _alien_texture.rect.w*_scaling, renderer->width, camera, 1.0),
		std::lround(_alien->alpha_pos.y * _scaling - camera.view_rect.y),
		_alien_texture.rect.w*_scaling,
		_alien_texture.rect.h*_scaling
	};
	SDL_RenderCopy(sdl_renderer, _alien_texture.texture, &_alien_texture.rect, &dstrect);
}