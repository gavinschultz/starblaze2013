#include "stdafx.h"
#include "Renderer.h"
#include "AlienSprite.h"

AlienSprite::AlienSprite(Renderer* renderer) : Sprite(renderer->scaling)
{
	this->_alien_texture = renderer->sprite_loader.getSprite("alien1");
}

void AlienSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera, const Alien& alien)
{
	SDL_Rect dstrect = 
	{
		renderutil::getScreenXForEntityByCameraAndDistance(alien.alpha_pos.x*_scaling, _alien_texture.rect.w*_scaling, renderer->width, camera, 1.0),
		std::lround(alien.alpha_pos.y * _scaling - camera.view_rect.y),
		_alien_texture.rect.w*_scaling,
		_alien_texture.rect.h*_scaling
	};

	if (alien.is_collided)
		SDL_SetTextureColorMod(_alien_texture.texture, 0, 0, 0);
	else
		SDL_SetTextureColorMod(_alien_texture.texture, 255, 255, 255);

	SDL_RenderCopy(sdl_renderer, _alien_texture.texture, &_alien_texture.rect, &dstrect);
	SDL_SetTextureColorMod(_alien_texture.texture, 255, 255, 255); // shared texture, so reset to defaults

	renderer->renderNormalVector({ dstrect.x + ((alien.bounding_box.w * (int)_scaling) / 2), dstrect.y + ((alien.bounding_box.h * (int)_scaling) / 2) }, alien.attack_vector_n);
}