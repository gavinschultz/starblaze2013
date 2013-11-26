#pragma once
#include <SDL.h>
#include <memory>
#include "Sprite.h"

class TextRenderer;
class Renderer;
class Ship;

class HUDRend : public Sprite
{
private:
	class impl;
	std::unique_ptr<impl> pimpl;
public:
	HUDRend(Renderer* renderer);
	~HUDRend();
	void render(SDL_Renderer* sdlRenderer, const TextRenderer& text_renderer, const Ship& ship, int score, int playerShips) const;
};