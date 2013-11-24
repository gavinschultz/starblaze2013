#include "stdafx.h"
#include <algorithm>
#include "Renderer.h"
#include "ShipSprite.h"
#include "Timer.h"

ShipSprite::ShipSprite(Renderer* renderer, Ship* ship) : Sprite()
{
	this->_scaling = renderer->scaling;

	this->_ship = std::shared_ptr<Ship>(ship);
	this->_stripe_texture_rect = { 0, 0, 32, 1 };
	this->_burner_texture_rect = { 0, 0, 8, 5 };
	//this->_burner_rev_texture_rect = { 0, 0, 4, 4 };

	this->_ship_texture = renderer->loadTextureFromFile("resources\\ship.tga", &this->_ship_texture_rect);
	this->_stripe_texture = renderer->loadTextureFromFile("resources\\shipstripe.tga", nullptr);
	this->_taillight_texture = renderer->loadTextureFromFile("resources\\shiptaillight.tga", &this->_taillight_texture_rect);
	this->_burner_texture = renderer->loadTextureFromFile("resources\\shipburner.tga", nullptr);
	//this->_burner_rev_texture = renderer->loadTextureFromFile("resources\\shipburner_rev.tga", nullptr);
	this->_wheels_texture = renderer->loadTextureFromFile("resources\\shipwheels.tga", &this->_wheels_texture_rect);

	this->_stripe_offset = { 0, 6 * (int32_t)_scaling };
	this->_burner_offset = { -_burner_texture_rect.w  * (int32_t)_scaling, (_ship_texture_rect.h - 5)  * (int32_t)_scaling };
	//this->_burner_rev_offset = { 17 * (int32_t)_scaling, 4 * (int32_t)_scaling };
	this->_wheels_offset = { 0, _ship_texture_rect.h * (int32_t)_scaling };

	this->smooth_animation = false;
}

ShipSprite::~ShipSprite()
{

}

void ShipSprite::render(SDL_Renderer* sdlRenderer, const Camera& camera)
{
	SDL_RendererFlip flip = _ship->direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RendererFlip flipReverse = flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	int32_t entity_x = renderutil::getScreenXForEntityByCameraAndDistance(_ship->alpha_pos.x*_scaling, _ship_texture_rect.w*_scaling, renderer->width, camera, 1.0);
	int32_t entity_y = std::lround(_ship->alpha_pos.y * _scaling - camera.view_rect.y);
	//SDL_Rect ship_rect = { entity_x, std::lround(_ship->alpha_pos.y * _scaling) - camera.view_rect.y, _ship_texture_rect.w  * _scaling, _ship_texture_rect.h  * _scaling };
	SDL_Rect ship_rect = { entity_x, entity_y, _ship_texture_rect.w  * _scaling, _ship_texture_rect.h  * _scaling };
	debug->set("_ship->alpha_pos.x", std::lround(_ship->alpha_pos.x*_scaling) - (double)(camera.view_rect.x * 1.0));
	debug->set("ship_rect.x", ship_rect.x);
	if (smooth_animation)
		_stripe_texture_rect.y = (timer->getTotalFrames() - 1) % 32;				// smooth scrolling stripe
	else
		_stripe_texture_rect.y = (((timer->getTotalFrames() - 1) % 32) / 8) * 8;	// "original" scrolling stripe}
	SDL_Rect stripe_rect = { ship_rect.x + _stripe_offset.x, ship_rect.y + _stripe_offset.y, _stripe_texture_rect.w  * _scaling, _stripe_texture_rect.h  * _scaling };

	SDL_Rect taillight_rect = {};
	SDL_Texture* taillight_texture = nullptr;
	if ((timer->getTotalFrames() - 1) % 20 < 10)
	{
		taillight_rect = { ship_rect.x, ship_rect.y, _taillight_texture_rect.w  * _scaling, _taillight_texture_rect.h  * _scaling };
		taillight_texture = this->_taillight_texture;
	}

	SDL_Rect burner_rect = {};
	SDL_Texture* burner_texture = nullptr; // may be either forward or reverse
	SDL_Rect burner_texture_rect = {};
	if (_ship->current_state.thrust.x != 0.0)
	{
		//_burner_texture_rect.x = std::min((int)(std::lround(_ship->max_thrust.x / std::abs(_ship->current_state.thrust.x))), 3) * 8;
		_burner_texture_rect.x = std::min(std::lround(4 * std::abs(_ship->current_state.thrust.x) / _ship->max_thrust.x) * 8, 24L);
		//_burner_rev_texture_rect.x = std::min((int)(abs(_ship->current_state.thrust.x) / 8.0), 1) * 4;
		//if ((_ship->current_state.thrust.x > 0.0 && _ship->direction == ShipDirection::right) || (_ship->current_state.thrust.x < 0.0 && _ship->direction == ShipDirection::left))
		if (_ship->current_state.thrust.x != 0.0)
		{
			burner_texture = _burner_texture;
			burner_texture_rect = _burner_texture_rect;
		}
		/*else
		{
		burner_texture = _burner_rev_texture;
		burner_texture_rect = _burner_rev_texture_rect;
		}*/

		if (_ship->current_state.thrust.x != 0.0 && _ship->direction == ShipDirection::right)
		{
			burner_rect = { ship_rect.x + _burner_offset.x, ship_rect.y + _burner_offset.y, _burner_texture_rect.w  * _scaling, _burner_texture_rect.h  * _scaling };
		}
		else if (_ship->current_state.thrust.x != 0.0 && _ship->direction == ShipDirection::left)
		{
			burner_rect = { ship_rect.x + ship_rect.w, ship_rect.y + _burner_offset.y, _burner_texture_rect.w  * _scaling, _burner_texture_rect.h  * _scaling };
		}

		/* reverse texture - disabled for now
		else if (_ship->current_state.thrust.x < 0.0 && _ship->direction == ShipDirection::right)
		{
		burner_rect = { ship_rect.x + _burner_rev_offset.x, ship_rect.y + _burner_rev_offset.y, _burner_rev_texture_rect.w  * _scaling, _burner_rev_texture_rect.h  * _scaling };
		}
		else if (_ship->current_state.thrust.x > 0.0 && _ship->direction == ShipDirection::left)
		{
		burner_rect = { ship_rect.x + ship_rect.w - (_burner_rev_texture_rect.w  * _scaling) - _burner_rev_offset.x, ship_rect.y + _burner_rev_offset.y, _burner_rev_texture_rect.w  * _scaling, _burner_rev_texture_rect.h  * _scaling };
		}
		*/
	}

	SDL_Texture* wheels_texture = nullptr;
	SDL_Rect wheels_rect = {};
	if (_ship->altitude == 0.0 && std::abs(_ship->current_state.vel.x) < _ship->takeoff_speed)
	{
		wheels_rect = { ship_rect.x + _wheels_offset.x, ship_rect.y + _wheels_offset.y, _wheels_texture_rect.w * _scaling, _wheels_texture_rect.h * _scaling };
		wheels_texture = this->_wheels_texture;
	}

	SDL_RenderCopyEx(sdlRenderer, _ship_texture, &_ship_texture_rect, &ship_rect, 0, NULL, flip);
	SDL_RenderCopyEx(sdlRenderer, _stripe_texture, &_stripe_texture_rect, &stripe_rect, 0, NULL, flipReverse);
	if (taillight_texture)
		SDL_RenderCopyEx(sdlRenderer, taillight_texture, &_taillight_texture_rect, &taillight_rect, 0, NULL, flip);
	if (burner_texture)
		SDL_RenderCopyEx(sdlRenderer, burner_texture, &burner_texture_rect, &burner_rect, 0, NULL, flip);
	/*if (burner_rev_texture)
	SDL_RenderCopyEx(sdlRenderer, burner_rev_texture, &burner_rev_texture_rect, &burner_rect, 0, NULL, flip);*/
	if (wheels_texture)
		SDL_RenderCopyEx(sdlRenderer, wheels_texture, &_wheels_texture_rect, &wheels_rect, 0, NULL, flip);
}