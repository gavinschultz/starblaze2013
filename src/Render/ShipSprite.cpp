#include "stdafx.h"
#include <algorithm>
#include "Renderer.h"
#include "ShipSprite.h"
#include "Entity\Ship.h"
#include "Timer.h"

ShipSprite::ShipSprite(Renderer* renderer) : Sprite(renderer->scaling)
{
	this->_stripe_texture = renderer->sprite_loader.getSprite("shipstripe_line");
	this->_wheels_texture = renderer->sprite_loader.getSprite("shipwheels");
	this->_taillight_texture = renderer->sprite_loader.getSprite("shiptaillight");	
	this->_burner_texture = renderer->sprite_loader.getSprite("shipburner");
	this->_ship_texture = renderer->sprite_loader.getSprite("ship");

	this->_stripe_offset = { 0, 6 * (int)_scaling };
	this->_burner_offset = { -BURNER_WIDTH  * (int)_scaling, (_ship_texture.rect.h - 5)  * (int)_scaling };
	this->_wheels_offset = { 0, _ship_texture.rect.h * (int)_scaling };

	this->smooth_animation = false;
}

ShipSprite::~ShipSprite() {}

void ShipSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera, const Ship& ship) const
{
	if (!ship.is_active)
		return;

	SDL_RendererFlip flip = ship.direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RendererFlip flipReverse = flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	int32_t entity_x = renderutil::getScreenXForEntityByCameraAndDistance(ship.state.interpolated.x*_scaling, _ship_texture.rect.w*_scaling, renderer->width, camera, 1.0);
	int32_t entity_y = std::lround(ship.state.interpolated.y * _scaling - camera.view_rect.y);
	SDL_Rect ship_rect = { entity_x, entity_y, _ship_texture.rect.w  * _scaling, _ship_texture.rect.h  * _scaling };
	
	SDL_Rect stripe_texture_rect = { _stripe_texture.rect.x, _stripe_texture.rect.y, SHIPSTRIPE_WIDTH, _stripe_texture.rect.h };
	if (smooth_animation)
		stripe_texture_rect.x += (timer->getTotalFrames() - 1) % 32;				// smooth scrolling stripe
	else
		stripe_texture_rect.x += (((timer->getTotalFrames() - 1) % 32) / 8) * 8;	// "original" scrolling stripe}
	SDL_Rect stripe_rect = { ship_rect.x + _stripe_offset.x, ship_rect.y + _stripe_offset.y, stripe_texture_rect.w * _scaling, stripe_texture_rect.h  * _scaling };

	SDL_Rect taillight_rect = {};
	SDL_Texture* taillight_texture = nullptr;
	if ((timer->getTotalFrames() - 1) % 20 < 10)
	{
		taillight_rect = { ship_rect.x, ship_rect.y, _taillight_texture.rect.w  * _scaling, _taillight_texture.rect.h  * _scaling };
		taillight_texture = _taillight_texture.texture;
	}

	SDL_Rect burner_rect = {};
	SDL_Texture* burner_texture = nullptr; // may be either forward or reverse
	SDL_Rect burner_texture_rect = { _burner_texture.rect.x, _burner_texture.rect.y, BURNER_WIDTH, _burner_texture.rect.h };
	if (ship.state.current.thrust.x != 0.0)
	{
		burner_texture_rect.x += std::min(std::lround(4 * std::abs(ship.state.current.thrust.x) / ship.max_thrust.x) * 8, 24L);
		if (ship.state.current.thrust.x != 0.0)
		{
			burner_texture = _burner_texture.texture;
		}

		if (ship.state.current.thrust.x != 0.0 && ship.direction == ShipDirection::right)
		{
			burner_rect = { ship_rect.x + _burner_offset.x, ship_rect.y + _burner_offset.y, burner_texture_rect.w  * _scaling, burner_texture_rect.h  * _scaling };
		}
		else if (ship.state.current.thrust.x != 0.0 && ship.direction == ShipDirection::left)
		{
			burner_rect = { ship_rect.x + ship_rect.w, ship_rect.y + _burner_offset.y, burner_texture_rect.w  * _scaling, burner_texture_rect.h  * _scaling };
		}
	}

	SDL_Texture* wheels_texture = nullptr;
	SDL_Rect wheels_rect = {};
	if (ship.isGearDown())
	{
		wheels_rect = { ship_rect.x + _wheels_offset.x, ship_rect.y + _wheels_offset.y, _wheels_texture.rect.w * _scaling, _wheels_texture.rect.h * _scaling };
		wheels_texture = _wheels_texture.texture;
	}

	if (ship.getCollidable().isCollided())
		SDL_SetTextureColorMod(_ship_texture.texture, 0, 0, 0);
	else
		SDL_SetTextureColorMod(_ship_texture.texture, 255, 255, 255);
	SDL_RenderCopyEx(sdl_renderer, _ship_texture.texture, &_ship_texture.rect, &ship_rect, 0, NULL, flip);
	SDL_SetTextureColorMod(_ship_texture.texture, 255, 255, 255); // shared texture, so reset to defaults

	SDL_RenderCopyEx(sdl_renderer, _stripe_texture.texture, &stripe_texture_rect, &stripe_rect, 0, NULL, flipReverse);
	if (taillight_texture)
		SDL_RenderCopyEx(sdl_renderer, taillight_texture, &_taillight_texture.rect, &taillight_rect, 0, NULL, flip);
	if (burner_texture)
		SDL_RenderCopyEx(sdl_renderer, burner_texture, &burner_texture_rect, &burner_rect, 0, NULL, flip);
	if (wheels_texture)
		SDL_RenderCopyEx(sdl_renderer, wheels_texture, &_wheels_texture.rect, &wheels_rect, 0, NULL, flip);
}