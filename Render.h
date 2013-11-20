#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <array>
#include <memory>
#include "Phys.h"
#include <memory>
#include "timer.h"
#include "Entity.h"
#include "Phys.h"
#include <SDL_ttf.h>
#include "Debug.h"
#include "Game.h"
#include <random>
#include <functional>

class Renderer;

class Window
{
public:
	uint32_t w;
	uint32_t h;
};

class Camera
{
public:
	Camera(const SDL_Rect& window_rect, const SDL_Rect& focus_rect);
	SDL_Rect view_rect;
	SDL_Rect focus_rect;
	SDL_Rect prev_focus_rect;
	Point2Di focus_point;
	int focus_loop_count{ 0 };
	int prev_focus_loop_count{ 0 };
	Vector2Di focus_point_vel;
	Point2Di prev_focus_point;
};

class Sprite
{
protected:
	uint32_t _scaling;
public:
	Sprite();
	~Sprite();
	virtual void render(SDL_Renderer* sdlRenderer, const Camera& camera) = 0;
};

class SpriteRegister
{
public:
	SpriteRegister();
	~SpriteRegister();
	void registerSprite(Sprite* sprite);
	std::vector<Sprite*> _sprites;
};

/*
The TRS-80 CoCo uses the Motorola 6847 VDG.

+ * GREEN = Y'UV(153,-128,-128) = RGB(7,277,-107) --> #07ff00
+ * YELLOW = Y'UV(255,-128,0) = RGB(255,305,-5) --> #ffff00
+ * BLUE = Y'UV(59,+128,0) = RGB(59,8,319) --> #3b08ff
+ * RED: Y'UV(59,0,+128) = RGB(204,-15,59) --> #cc003b
+ * WHITE: Y'UV(255,0,0) = RGB(255,255,255) --> #ffffff
+ * CYAN: Y'UV(153,0,-128) = RGB(7,227,153) --> #07e399
+ * MAGENTA: Y'UV(153,+128,+128) = RGB(298,28,413) --> #ff1cff
+ * ORANGE: Y'UV(153,-128,+128) = RGB(298,129,-107) --> #ff8100
+ * BLACK: Y'UV(0,0,0) = RGB(0,0,0) --> #000000
+ * DARK GREEN: Y'UV(0,-128,-128) = RGB(-145,124,-260) --> #007c00
+ * DARK ORANGE: Y'UV(0,-128,+128) = RGB(145,-23,-260) --> #910000

From http://forums.bannister.org/ubbthreads.php?ubb=showflat&Number=64986&PHPSESSID=3c74d06c904029be10e5bfea6062aeab
*/

enum CoCoPaletteEnum
{
	green,
	yellow,
	blue,
	red,
	white,
	cyan,
	magenta,
	orange,
	black,
	darkgreen,
	darkorange
};

class CoCoPalette
{
private:
	std::unordered_map<CoCoPaletteEnum, SDL_Color> coco_palette = { {
		{ green, { 7, 255, 0, 255 } },
		{ yellow, { 255, 255, 0, 255 } },
		{ blue, { 59, 8, 255, 255 } },
		{ red, { 204, 0, 59, 255 } },
		{ white, { 255, 255, 255, 255 } },
		{ cyan, { 7, 227, 153, 255 } },
		{ magenta, { 255, 28, 255, 255 } },
		{ orange, { 255, 129, 0, 255 } },
		{ black, { 0, 0, 0, 255 } },
		{ darkgreen, { 0, 124, 0, 255 } },
		{ darkorange, { 145, 0, 0, 255 } }
	} };
public:
	const SDL_Color& getColor(const CoCoPaletteEnum color) const;
};

class TextLine
{
public:
	TextLine(const std::string& text, Point2Di offset);
	const std::string text;
	const Point2Di offset;
};

class TextPlate
{
public:
	TextPlate(std::initializer_list<TextLine> lines);
	std::vector<TextLine> lines;
};

class TextRenderer
{
public:
	TextRenderer(Renderer* renderer);
	void RenderChar(SDL_Renderer* sdl_renderer, char letter, Point2Di pos, bool red = false) const;
	void RenderString(SDL_Renderer* sdl_renderer, const std::string& text, Point2Di pos, bool red = false) const;
	void RenderPlate(SDL_Renderer* sdl_renderer, const TextPlate& plate) const;
private:
	const uint32_t _scaling;
	const uint32_t _spacing{ 0 };	// character spacing in unscaled pixels
	static const std::unordered_map<char, Point2Di> _char_offsets;
	SDL_Rect _charmap_texture_rect;
	SDL_Texture* _charmap_texture;
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
	Renderer(uint32_t screen_width, uint32_t screen_height, uint32_t scaling, double world_width);
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
	const uint32_t width;
};

class ShipSprite : public Sprite
{
private:
	std::shared_ptr<Ship> _ship;	// TODO: weak_ptr / shared_ptr?
	SDL_Texture* _ship_texture;
	SDL_Texture* _taillight_texture;
	SDL_Texture* _stripe_texture;
	SDL_Texture* _burner_texture;
	//SDL_Texture* _burner_rev_texture;
	SDL_Texture* _wheels_texture;
	SDL_Rect _ship_texture_rect;
	SDL_Rect _taillight_texture_rect;
	SDL_Rect _stripe_texture_rect;
	SDL_Rect _burner_texture_rect;
	//SDL_Rect _burner_rev_texture_rect;
	SDL_Rect _wheels_texture_rect;

	Point2Di _stripe_offset;
	Point2Di _burner_offset;
	//Point2Di _burner_rev_offset;
	Point2Di _wheels_offset;
public:
	ShipSprite(Renderer* renderer, Ship* ship);
	~ShipSprite();
	void render(SDL_Renderer* sdlRenderer, const Camera& camera);
	bool smooth_animation;
};

class BGSprite : public Sprite
{
private:
	std::shared_ptr<World> _world;
	SDL_Texture* _hills_texture;
	SDL_Rect _hills_texture_rect;
	SDL_Rect _hills_rect;

	SDL_Rect _sky_rect;
	SDL_Color _sky_color;

	SDL_Rect _ground_rect;
	SDL_Color _ground_color;

	SDL_Rect _bg_rect;
	SDL_Color _bg_color;
public:
	BGSprite(Renderer* renderer, World* world);
	~BGSprite();
	void render(SDL_Renderer* sdlRenderer, const Camera& camera);
};

class RadarSprite : public Sprite
{
private:
	SDL_Rect _radar_rect;
	std::array<Point2Di, 4> _view_points;
	SDL_Color _radar_color;
	SDL_Color _point_color;
public:
	RadarSprite(Renderer* renderer);
	void render(SDL_Renderer* sdlRenderer, const Camera& camera);
};

class StationSprite : public Sprite
{
private:
	std::shared_ptr<Station> station;	//TODO: would rather not store reference here, should pass in at render time
	SDL_Texture* _station_texture;
	SDL_Rect _station_texture_rect;
	int station_type;	// 0 = fuel, 1 = repair
public:
	StationSprite(Renderer* renderer);
	~StationSprite();
	void render(SDL_Renderer* sdlRenderer, const Camera& camera);
};

namespace render
{
	int32_t getScreenXForEntityByCameraAndDistance(double entity_x_at_zero, uint32_t entity_sprite_width, uint32_t world_width, const Camera& camera, double distance_factor);
}

extern std::unique_ptr<Renderer> renderer;