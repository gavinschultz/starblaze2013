#pragma once
#include <SDL.h>
#include <vector>
#include "Phys.h"
#include <memory>
#include "timer.h"
#include "Entity.h"
#include "Phys.h"

class Window
{
public:
	int w;
	int h;
};

class Sprite
{
public:
	Sprite();
	~Sprite();
	Point2D world_pos;
	virtual void render(SDL_Renderer* sdlRenderer) = 0;
};

class SpriteRegister
{
private:
	SDL_Rect _world_rect;
public:
	SpriteRegister();
	~SpriteRegister();
	void registerSprite(Sprite* sprite);
	std::vector<Sprite*> _sprites;
};

class Renderer
{
private:
	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	void init();
public:
	Renderer();
	Renderer(int screen_width, int screen_height, float scaling);
	~Renderer();
	SpriteRegister sprite_register;
	void render();
	SDL_Texture* loadTextureFromFile(std::string imagePath);
	Window window;
	float scaling;
};

class ShipSprite : public Sprite
{
private:
	std::shared_ptr<Ship> _ship;	// TODO: weak_ptr / shared_ptr?
	SDL_Texture* _ship_texture;
	SDL_Texture* _taillight_texture;
	SDL_Texture* _stripe_texture;
	SDL_Texture* _burner_texture;
	SDL_Texture* _burner_rev_texture;
	SDL_Rect _ship_texture_rect;
	SDL_Rect _taillight_texture_rect;
	SDL_Rect _stripe_texture_rect;
	SDL_Rect _burner_texture_rect;
	SDL_Rect _burner_rev_texture_rect;

	Point2D _stripe_offset;
	Point2D _burner_offset;
	Point2D _burner_rev_offset;
	float _scaling;
public:
	ShipSprite(Renderer* renderer, Ship* ship);
	~ShipSprite();
	void render(SDL_Renderer* sdlRenderer);
	bool smooth_animation = true;
};