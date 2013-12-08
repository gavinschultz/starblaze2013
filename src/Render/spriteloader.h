#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <SDL.h>

class SpriteTexture
{
public:
	SDL_Texture* texture;
	SDL_Rect rect;
};

class SpriteLoader
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	SpriteLoader();
	~SpriteLoader();
	void load(SDL_Renderer* sdl_renderer, const std::string& spritesheet_path);
	const SpriteTexture& getSprite(const std::string& name) const;
};