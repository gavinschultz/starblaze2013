#pragma once
#include "Sprite.h"
#include "SpriteLoader.h"

class Renderer;
class World;
class Camera;

class BGSprite : public Sprite
{
private:
	const int HILL_HEIGHT = 4;
	const int HILL_WIDTH = 16;
	const int SKY_HEIGHT = 96;
	const int GROUND_HEIGHT = 48;

	SpriteTexture _hills_texture;

	SDL_Rect _sky_rect;
	SDL_Color _sky_color;

	SDL_Rect _ground_rect;
	SDL_Color _ground_color;

	SDL_Rect _bg_rect;
	SDL_Color _bg_color;
public:
	BGSprite(Renderer* renderer);
	~BGSprite();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const World& world) const;
};