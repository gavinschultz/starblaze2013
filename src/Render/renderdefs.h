#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <vector>
#include <map>
#include "program.h"
#include "component.h"
#include "phys.h"
#include "textrender.h"

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
	void render(SDL_Renderer* sdl_renderer, Window window, Rect playfield_rect) const;
};

class ZeroLineRender
{
public:
	void render(SDL_Renderer* sdl_renderer, Window window, const Camera& camera) const;
};

class DebugRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	DebugRender(TTF_Font* font);
	~DebugRender();
	void render(SDL_Renderer* sdl_renderer, const std::map<std::string, std::string>& debug_items) const;
};

class MotionHistoryRender
{
public:
	void render(SDL_Renderer* sdl_renderer, Window window, const std::vector<Vector2D>& points, Vector2D thresholds, unsigned int current_index) const;
};

class ShipRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	ShipRender(const RenderSystem& renderer);
	~ShipRender();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent& state, const HorizontalOrientComponent& orient, const ThrustComponent& thrust, const PhysicalComponent& phys, const CollisionComponent* collide) const;
};

class AlienRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	AlienRender(const RenderSystem& renderer);
	~AlienRender();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent& state, const CollisionComponent* collide) const;
};

class HUDRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	HUDRender(const RenderSystem& renderer);
	~HUDRender();
	void render(SDL_Renderer* sdl_renderer, const PlayerComponent& player, unsigned int score, unsigned int lives, const TextRender& text_renderer) const;
};

class RadarRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	RadarRender(const RenderSystem& renderer);
	~RadarRender();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent* state, const PhysicalComponent* physical) const;
	void renderBox(SDL_Renderer* sdl_renderer);
};

class CollisionBoxRender
{
public:
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const CollisionComponent& c) const;
};

class StationRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	StationRender(const RenderSystem& renderer);
	~StationRender();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const StationComponent& station, const TemporalState2DComponent& state, const PhysicalComponent& phys) const;
};

class BulletRender
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	BulletRender(const RenderSystem& renderer);
	~BulletRender();
	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent& state, PhysicalComponent& physical) const;
};