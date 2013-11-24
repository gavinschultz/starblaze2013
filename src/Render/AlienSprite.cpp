#include "stdafx.h"
#include "Renderer.h"
#include "AlienSprite.h"

AlienSprite::AlienSprite(Renderer* renderer, Alien* alien)
{
	this->_scaling = renderer->scaling;
	this->_alien = alien;
	this->_alien_texture = renderer->loadTextureFromFile("resources\\alien1.tga", nullptr);
	this->_alien_texture_rect = { 0, 0, 16, 12 };
}

void AlienSprite::render(SDL_Renderer* sdl_renderer, const Camera &camera)
{
	SDL_Rect srcrect = _alien_texture_rect;
	SDL_Rect dstrect = {
		renderutil::getScreenXForEntityByCameraAndDistance(_alien->alpha_pos.x*_scaling, _alien_texture_rect.w*_scaling, renderer->width, camera, 1.0),
		std::lround(_alien->alpha_pos.y * _scaling - camera.view_rect.y),
		_alien_texture_rect.w*_scaling,
		_alien_texture_rect.h*_scaling
	};
	SDL_RenderCopy(sdl_renderer, _alien_texture, &srcrect, &dstrect);
}