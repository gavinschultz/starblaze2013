#pragma once
#include <SDL.h>

class MappedTexture
{
public:
	MappedTexture(int scaling = 1) {}
	MappedTexture(SDL_Texture* texture, SDL_Rect rect, int scaling = 1) : texture{ texture }, scaling_{ scaling }
	{
		this->rect = rect;	// doesn't seem to work in ctor data member initialization, possibly not yet supported
	}
	SDL_Texture* texture;
	SDL_Rect rect;
	SDL_Rect getScaledRect() const;
private:
	int scaling_{ 1 };
};