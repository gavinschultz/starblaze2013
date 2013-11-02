#include "Renderer.h"
#include "Entity.h"
#include "Debug.h"
#include <SDL.h>
#include <SDL_image.h>

Renderer::Renderer(int window_width, int window_height, float scaling)
{
	this->window.w = window_width;
	this->window.h = window_height;
	this->scaling = 0.5;
	init();
}

Renderer::~Renderer()
{
	this->window.w = 1024;
	this->window.h = 768;
	this->scaling = 1.0;
	init();
}

void Renderer::init()
{
	sdlWindow = SDL_CreateWindow("Starblaze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.w * scaling, window.h * scaling, SDL_WINDOW_BORDERLESS);// SDL_WINDOW_FULLSCREEN_DESKTOP);
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
		SDL_Rect spriteRect = { (int)sprite->world_pos.x, (int)sprite->world_pos.y, sprite->rect.w * scaling, sprite->rect.h * scaling };
		SDL_RenderCopyEx(sdlRenderer, sprite->texture, NULL, &spriteRect, 0, NULL, sprite->flip);
	}

	/*
	SDL_Rect shipstripeTextureRectangle;
	if (game->smooth_motion)
		shipstripeTextureRectangle = { 0, (ptimer->getTotalFrames() - 1) % 32, 32, 1 };	// smooth scrolling stripe
	else
		shipstripeTextureRectangle = { 0, (((ptimer->getTotalFrames() - 1) % 32) / 8) * 8, 32, 1 };	// "original" scrolling stripe

	SDL_Rect shipstripeRect = { shipRect.x, shipRect.y + (6 * game->window.scaling), 32 * game->window.scaling, 1 * game->window.scaling };
	SDL_RenderCopyEx(renderer, shipstripeTexture, &shipstripeTextureRectangle, &shipstripeRect, 0, NULL, ship->direction == ShipDirection::right ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

	if ((ptimer->getTotalFrames() - 1) % 20 < 10)
	{
		SDL_Rect shiptaillightTextureRectangle = { 0, 0, 32, 1 };
		SDL_Rect shiptaillightRect = { shipRect.x, shipRect.y, shiptaillightTextureRectangle.w * game->window.scaling, shiptaillightTextureRectangle.h * game->window.scaling };
		SDL_RenderCopyEx(renderer, shiptaillightTexture, &shiptaillightTextureRectangle, &shiptaillightRect, 0, NULL, ship->direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}

	debug({ "Ship thrust/vel X:", std::to_string(ship->current_state.thrust.x), "/", std::to_string(ship->current_state.vel.x), " accel Y:", std::to_string(ship->current_state.acc.y), " direction:", std::to_string((int)ship->direction) });
	if (ship->current_state.thrust.x != 0.0)
	{
		SDL_Rect shipburnerTextureRect = { min((int)(abs(ship->current_state.thrust.x) / 12.0), 3) * 8, 0, 8, 5 };
		SDL_Rect shipburnerRevTextureRect = { min((int)(abs(ship->current_state.thrust.x) / 8.0), 1) * 4, 0, 4, 4 };
		SDL_Rect shipburnerRect;
		if (ship->current_state.thrust.x > 0.0 && ship->direction == ShipDirection::right)
		{
			//debug({ "Flare RIGHT" });
			shipburnerRect = { shipRect.x - (shipburnerTextureRect.w * game->window.scaling), shipRect.y + (shipRect.h - (shipburnerTextureRect.h * game->window.scaling)), shipburnerTextureRect.w * game->window.scaling, shipburnerTextureRect.h * game->window.scaling };
			SDL_RenderCopyEx(renderer, shipburnerTexture, &shipburnerTextureRect, &shipburnerRect, 0, NULL, ship->direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
		}
		else if (ship->current_state.thrust.x < 0.0 && ship->direction == ShipDirection::left)
		{
			//debug({ "Flare LEFT" });
			shipburnerRect = { shipRect.x + shipRect.w, shipRect.y + (shipRect.h - (shipburnerTextureRect.h * game->window.scaling)), shipburnerTextureRect.w * game->window.scaling, shipburnerTextureRect.h * game->window.scaling };
			SDL_RenderCopyEx(renderer, shipburnerTexture, &shipburnerTextureRect, &shipburnerRect, 0, NULL, ship->direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
		}
		else if (ship->current_state.thrust.x < 0.0 && ship->direction == ShipDirection::right)
		{
			//debug({ "REV Flare RIGHT" });
			shipburnerRect = { shipRect.x + (17 * game->window.scaling), shipRect.y + (4 * game->window.scaling), shipburnerRevTextureRect.w * game->window.scaling, shipburnerRevTextureRect.h * game->window.scaling };
			SDL_RenderCopyEx(renderer, shipburnerRevTexture, &shipburnerRevTextureRect, &shipburnerRect, 0, NULL, ship->direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
		}
		else if (ship->current_state.thrust.x > 0.0 && ship->direction == ShipDirection::left)
		{
			//debug({ "REV Flare LEFT" });
			shipburnerRect = { shipRect.x + (15 * game->window.scaling), shipRect.y + (4 * game->window.scaling), shipburnerRevTextureRect.w * game->window.scaling, shipburnerRevTextureRect.h * game->window.scaling };
			SDL_RenderCopyEx(renderer, shipburnerRevTexture, &shipburnerRevTextureRect, &shipburnerRect, 0, NULL, ship->direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
		}
	}
	*/
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