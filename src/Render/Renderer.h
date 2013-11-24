#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Debug.h"
#include "Camera.h"
#include "TextRenderer.h"
#include "RenderUtil.h"
#include "CoCoPalette.h"
#include "SpriteRegister.h"

class Window
{
public:
	uint32_t w;
	uint32_t h;
};

class Renderer
{
private:
	enum GaugeType
	{
		Fuel = 0,
		Bullets = 1,
		Shields = 2,
		Radar = 3
	};

	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	void renderGrid();
	void renderFPS(int fps);
	void renderDebug(const Debug& debug);
	void renderMotionHistory(const Debug& debug);
	void renderText(const std::string text, uint32_t x, uint32_t y);
	void renderZeroLine(const Camera& camera);
	void renderHUD();
	void init();
	TTF_Font* _font;
	std::unique_ptr<TextRenderer> _text_renderer;
	std::shared_ptr<TextPlate> _text_plate;
	//auto rnd_burner = std::bind(std::uniform_int_distribution<int>{0, 3}, std::default_random_engine{});
public:
	Renderer(unsigned int screen_width, unsigned int screen_height, unsigned int scaling, double world_width);
	~Renderer();
	SpriteRegister sprite_register;
	CoCoPalette coco_palette;
	void render(Camera* camera);
	void renderTextPlate(std::shared_ptr<TextPlate> text_plate);
	SDL_Texture* loadTextureFromFile(std::string imagePath, SDL_Rect* texture_rect);

	Window window;
	const uint32_t scaling;
	bool is_fullscreen{ false };
	bool is_grid_visible{ false };
	bool is_motionhistory_visible{ false };
	void toggleFullscreen(bool state);
	void toggleGrid(bool state);
	void toggleMotionHistory(bool state);
	bool isRightOf(int32_t x, int32_t y);
	bool isLeftOf(int32_t x, int32_t y);
	const unsigned int width;
};

extern std::unique_ptr<Renderer> renderer;