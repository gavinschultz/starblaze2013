#pragma once
#include <SDL.h>
#include <vector>
#include "Phys.h"
#include <memory>

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
	SDL_Rect rect;
	Point2D world_pos;
	SDL_Texture* texture;
	SDL_RendererFlip flip;
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

