#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include "program.h"
#include "component.h"
#include "phys.h"
#include "textrender.h"

class RenderSystem;
class PlayField;
class Camera;
//class TextRender;
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
	void render(SDL_Renderer* sdl_renderer, Window window, Rect playfield_rect) const;
};

class DebugRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	DebugRender(TTF_Font* font);
	~DebugRender();
	void render(SDL_Renderer* sdl_renderer, const std::unordered_map<std::string, debug::DebugItem>& debug_items);
};

class ShipRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	ShipRender(const RenderSystem& renderer);
	~ShipRender();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent& state, const HorizontalOrientComponent& orient, const ThrustComponent& thrust);
};

class HUDRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	HUDRender(const RenderSystem& renderer);
	~HUDRender();
	void render(SDL_Renderer* sdl_renderer, const PlayerComponent& player, unsigned int score, unsigned int lives, const TextRender& text_renderer);
};

class RadarRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	RadarRender(const RenderSystem& renderer);
	~RadarRender();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const RadarTrackableComponent& c);
	void renderBox(SDL_Renderer* sdl_renderer);
};

//for (auto c : db->getComponentsOfType(C::radartrackable))
//{
//	radar_render.draw(pi->sdl_renderer, camera, (RadarTrackable*)c);
//}