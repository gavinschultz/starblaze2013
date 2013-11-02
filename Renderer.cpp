#include "Renderer.h"
#include "Entity.h"
#include "Debug.h"
#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>

Renderer::Renderer(int window_width, int window_height, float scaling)
{
	this->window.w = window_width;
	this->window.h = window_height;
	this->scaling = scaling;
	init();
}

Renderer::Renderer()
{
	this->window.w = 1024;
	this->window.h = 768;
	this->scaling = 1.0;
	init();
}

Renderer::~Renderer(){}

void Renderer::init()
{
	sdlWindow = SDL_CreateWindow("Starblaze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.w, window.h, SDL_WINDOW_BORDERLESS);// SDL_WINDOW_FULLSCREEN_DESKTOP);
	debug({ SDL_GetError() });
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	debug({ SDL_GetError() });
}

SDL_Texture* Renderer::loadTextureFromFile(std::string imagePath)
{
	SDL_Surface* sdlSurface = IMG_Load(imagePath.c_str());
	debug({ SDL_GetError() });
	SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlSurface);
	SDL_FreeSurface(sdlSurface);
	SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
	return sdlTexture;
}

void Renderer::render()
{
	SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdlRenderer);

	for (auto& sprite : sprite_register._sprites)
	{
		sprite->render(this->sdlRenderer);
	}

	SDL_RenderPresent(sdlRenderer);
}

SpriteRegister::SpriteRegister()
{
	_world_rect = { 0, 0, 2048, 1536 };
}

SpriteRegister::~SpriteRegister()
{

}

void SpriteRegister::registerSprite(Sprite* sprite)
{
	this->_sprites.push_back(sprite);
}

Sprite::Sprite()
{

}

Sprite::~Sprite()
{

}

ShipSprite::ShipSprite(Renderer* renderer, Ship* ship) : Sprite()
{
	this->_scaling = renderer->scaling;

	this->_ship = std::shared_ptr<Ship>(ship);
	this->_ship_texture_rect = { 0, 0, 32, 8 };
	this->_stripe_texture_rect = { 0, 0, 32, 1 };
	this->_taillight_texture_rect = { 0, 0, 32, 1 };
	this->_burner_texture_rect = { 0, 0, 8, 5 };
	this->_burner_rev_texture_rect = { 0, 0, 4, 4 };
	this->_stripe_offset = { 0, 6 * 8 * _scaling };
	this->_burner_offset = { -_burner_texture_rect.w * 8 * _scaling, (_ship_texture_rect.h - 5) * 8 * _scaling };
	this->_burner_rev_offset = { 17 * 8 * _scaling, 4 * 8 * _scaling };

	this->_ship_texture = renderer->loadTextureFromFile("img\\ship.tga");
	this->_stripe_texture = renderer->loadTextureFromFile("img\\shipstripe.tga");
	this->_taillight_texture = renderer->loadTextureFromFile("img\\shiptaillight.tga");
	this->_burner_texture = renderer->loadTextureFromFile("img\\shipburner.tga");
	this->_burner_rev_texture = renderer->loadTextureFromFile("img\\shipburner_rev.tga");
}

ShipSprite::~ShipSprite()
{
	
}

void ShipSprite::render(SDL_Renderer* sdlRenderer)
{
	world_pos.x = _ship->alpha_state.pos.x;
	world_pos.y = _ship->alpha_state.pos.y;
	SDL_RendererFlip flip = _ship->direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RendererFlip flipReverse = flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	SDL_Rect ship_rect = { (int)world_pos.x, (int)world_pos.y, _ship_texture_rect.w * 8 * _scaling, _ship_texture_rect.h * 8 * _scaling };

	if (smooth_animation)
		_stripe_texture_rect.y = (timer->getTotalFrames() - 1) % 32;				// smooth scrolling stripe
	else
		_stripe_texture_rect.y = (((timer->getTotalFrames() - 1) % 32) / 8) * 8;	// "original" scrolling stripe}

	SDL_Rect stripe_rect = { ship_rect.x + _stripe_offset.x, ship_rect.y + _stripe_offset.y, _stripe_texture_rect.w * 8 * _scaling, _stripe_texture_rect.h * 8 * _scaling };

	SDL_Rect taillight_rect = {};
	if ((timer->getTotalFrames() - 1) % 20 < 10)
	{
		taillight_rect = { ship_rect.x, ship_rect.y, _taillight_texture_rect.w * 8 * _scaling, _taillight_texture_rect.h * 8 * _scaling };
	}

	SDL_RenderCopyEx(sdlRenderer, _ship_texture, &_ship_texture_rect, &ship_rect, 0, NULL, flip);
	SDL_RenderCopyEx(sdlRenderer, _stripe_texture, &_stripe_texture_rect, &stripe_rect, 0, NULL, flipReverse);
	if (taillight_rect.w > 0)
		SDL_RenderCopyEx(sdlRenderer, _taillight_texture, &_taillight_texture_rect, &taillight_rect, 0, NULL, flip);

	debug({ "Ship thrust/vel X:", std::to_string(_ship->current_state.thrust.x), "/", std::to_string(_ship->current_state.vel.x), " accel Y:", std::to_string(_ship->current_state.acc.y), " direction:", std::to_string((int)_ship->direction) });
	if (_ship->current_state.thrust.x != 0.0)
	{
		_burner_texture_rect.x = std::min((int)(abs(_ship->current_state.thrust.x) / 12.0), 3) * 8;
		_burner_rev_texture_rect.x = std::min((int)(abs(_ship->current_state.thrust.x) / 8.0), 1) * 4;
		SDL_Rect burner_rect;
		SDL_Texture* burner_texture; // may be either forward or reverse
		SDL_Rect texture_rect;
		if ((_ship->current_state.thrust.x > 0.0 && _ship->direction == ShipDirection::right) || (_ship->current_state.thrust.x < 0.0 && _ship->direction == ShipDirection::left))
		{
			burner_texture = _burner_texture;
			texture_rect = _burner_texture_rect;
		}
		else
		{
			burner_texture = _burner_rev_texture;
			texture_rect = _burner_rev_texture_rect;
		}

		if (_ship->current_state.thrust.x > 0.0 && _ship->direction == ShipDirection::right)
		{
			//debug({ "Flare RIGHT" });
			burner_rect = { ship_rect.x + _burner_offset.x, ship_rect.y + _burner_offset.y, _burner_texture_rect.w * 8 * _scaling, _burner_texture_rect.h * 8 * _scaling };
		}
		else if (_ship->current_state.thrust.x < 0.0 && _ship->direction == ShipDirection::left)
		{
			//debug({ "Flare LEFT" });
			burner_rect = { ship_rect.x + ship_rect.w, ship_rect.y + _burner_offset.y, _burner_texture_rect.w * 8 * _scaling, _burner_texture_rect.h * 8 * _scaling };
		}
		else if (_ship->current_state.thrust.x < 0.0 && _ship->direction == ShipDirection::right)
		{
			//debug({ "REV Flare RIGHT" });
			burner_rect = { ship_rect.x + _burner_rev_offset.x, ship_rect.y + _burner_rev_offset.y, _burner_rev_texture_rect.w * 8 * _scaling, _burner_rev_texture_rect.h * 8 * _scaling };
		}
		else if (_ship->current_state.thrust.x > 0.0 && _ship->direction == ShipDirection::left)
		{
			//debug({ "REV Flare LEFT" });
			burner_rect = { ship_rect.x + ship_rect.w - (_burner_rev_texture_rect.w * 8 * _scaling) - _burner_rev_offset.x, ship_rect.y + _burner_rev_offset.y, _burner_rev_texture_rect.w * 8 * _scaling, _burner_rev_texture_rect.h * 8 * _scaling };
		}
		SDL_RenderCopyEx(sdlRenderer, burner_texture, &texture_rect, &burner_rect, 0, NULL, flip);
	}
}