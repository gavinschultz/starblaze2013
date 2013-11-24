#pragma once
#include <SDL.h>
#include "Entity.h"
#include "Phys.h"
#include "Sprite.h"
#include "Renderer.h"

class ShipSprite : public Sprite
{
private:
	std::shared_ptr<Ship> _ship;	// TODO: weak_ptr / shared_ptr?
	SDL_Texture* _ship_texture;
	SDL_Texture* _taillight_texture;
	SDL_Texture* _stripe_texture;
	SDL_Texture* _burner_texture;
	SDL_Texture* _wheels_texture;
	SDL_Rect _ship_texture_rect;
	SDL_Rect _taillight_texture_rect;
	SDL_Rect _stripe_texture_rect;
	SDL_Rect _burner_texture_rect;
	SDL_Rect _wheels_texture_rect;

	Point2Di _stripe_offset;
	Point2Di _burner_offset;
	Point2Di _wheels_offset;
public:
	ShipSprite(Renderer* renderer, Ship* ship);
	~ShipSprite();
	void render(SDL_Renderer* sdlRenderer, const Camera& camera);
	bool smooth_animation;
};