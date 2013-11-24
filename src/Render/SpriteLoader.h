#pragma once
#include <string>
#include <unordered_map>

class SpriteTexture
{
public:
	SDL_Texture* texture;
	SDL_Rect rect;
	SDL_Rect srect;	// affine-transformed rect
	void scaleRect(unsigned int scale);
};

class SpriteLoader
{
public:
	SpriteLoader();
	void load(SDL_Renderer* sdl_renderer, const std::string& spritesheet_path);
	const SpriteTexture& getSprite(const std::string& name);
private:
	SDL_Texture* _texture = nullptr;	// only support for one texture (i.e. one spritesheet) at the moment
	SDL_Rect _texture_rect;
	std::unordered_map<std::string, SpriteTexture> _sprites_by_name;
};