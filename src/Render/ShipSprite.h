#pragma once
#include <SDL.h>
#include "Phys.h"
#include "Sprite.h"

class Renderer;
class Camera;
class Ship;

class ShipSprite : public Sprite
{
private:
	const int SHIPSTRIPE_WIDTH = 32;
	const int BURNER_WIDTH = 8;

	SpriteTexture _stripe_texture;
	SpriteTexture _wheels_texture;
	SpriteTexture _taillight_texture;
	SpriteTexture _burner_texture;
	SpriteTexture _ship_texture;

	Point2Di _stripe_offset;
	Point2Di _burner_offset;
	Point2Di _wheels_offset;
public:
	ShipSprite(Renderer* renderer);
	~ShipSprite();
	void render(SDL_Renderer* sdlRenderer, const Camera& camera, const Ship& ship) const;
	bool smooth_animation;
};