#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <vector>
#include "program.h"

class RenderSystem;
class PlayField;
class Camera;
struct Window;

class BackgroundRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	BackgroundRender(const RenderSystem& renderer);
	~BackgroundRender();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const PlayField& playfield) const;
};

class FPSRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	FPSRender(TTF_Font* font, const Window& window);
	~FPSRender();
	void render(SDL_Renderer* sdl_renderer, double fps) const;
};

class GridRender
{
public:
	void render(SDL_Renderer* sdl_renderer) const;
};

class DebugRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	DebugRender(TTF_Font* font);
	~DebugRender();
	void render(SDL_Renderer* sdl_renderer, const std::vector<debug::DebugItem>& debug_items);
};