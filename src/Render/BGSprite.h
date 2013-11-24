#pragma once
#include <memory>
#include "Sprite.h"
#include "Entity.h"
#include "Renderer.h"

class BGSprite : public Sprite
{
private:
	std::shared_ptr<World> _world;
	SDL_Texture* _hills_texture;
	SDL_Rect _hills_texture_rect;
	SDL_Rect _hills_rect;

	SDL_Rect _sky_rect;
	SDL_Color _sky_color;

	SDL_Rect _ground_rect;
	SDL_Color _ground_color;

	SDL_Rect _bg_rect;
	SDL_Color _bg_color;
public:
	BGSprite(Renderer* renderer, World* world);
	~BGSprite();
	void render(SDL_Renderer* sdlRenderer, const Camera& camera);
};