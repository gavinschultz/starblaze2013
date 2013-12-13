#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <SDL.h>
#include "MappedTexture.h"

class SpriteLoader
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	SpriteLoader();
	~SpriteLoader();
	void load(SDL_Renderer* sdl_renderer, const std::string& spritesheet_path);
	const MappedTexture& getSprite(const std::string& name) const;
};