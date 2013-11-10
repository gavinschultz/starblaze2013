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

class Window
{
public:
	int w;
	int h;
};

class Camera
{
public:
	Camera(const SDL_Rect& window_rect, const SDL_Rect& focus_rect);
	SDL_Rect view_rect;
	//SDL_Rect prev_view_rect;
	SDL_Rect focus_rect;
	Point2Di focus_point;
	Vector2Di focus_point_vel;
	Point2Di prev_focus_point;

	//uint32_t wrap_x;
};

class Sprite
{
protected:
	float _scaling;
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

class Renderer
{
private:
	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	SDL_Rect _viewport_rect;
	void renderGrid();
	void renderFPS(int fps);
	void renderDebug(const Debug& debug);
	void renderText(const std::string text, uint32_t x, uint32_t y);
	void init();
	TTF_Font* _font;
public:
	Renderer(int screen_width, int screen_height, float scaling);
	~Renderer();
	SpriteRegister sprite_register;
	CoCoPalette coco_palette;
	void render(Camera* camera);
	SDL_Texture* loadTextureFromFile(std::string imagePath, SDL_Rect* texture_rect);

	Window window;
	uint32_t scaling;
	bool is_fullscreen = false;
	bool is_grid_visible = false;
	void toggleFullscreen(bool state);
	void toggleGrid(bool state);
};

class ShipSprite : public Sprite
{
private:
	std::shared_ptr<Ship> _ship;	// TODO: weak_ptr / shared_ptr?
	SDL_Texture* _ship_texture;
	SDL_Texture* _taillight_texture;
	SDL_Texture* _stripe_texture;
	SDL_Texture* _burner_texture;
	SDL_Texture* _burner_rev_texture;
	SDL_Texture* _wheels_texture;
	SDL_Rect _ship_texture_rect;
	SDL_Rect _taillight_texture_rect;
	SDL_Rect _stripe_texture_rect;
	SDL_Rect _burner_texture_rect;
	SDL_Rect _burner_rev_texture_rect;
	SDL_Rect _wheels_texture_rect;

	Point2D _stripe_offset;
	Point2D _burner_offset;
	Point2D _burner_rev_offset;
	Point2D _wheels_offset;
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
	std::array<Point2D, 4> _view_points;
	SDL_Color _radar_color;
	SDL_Color _point_color;
public:
	RadarSprite(Renderer* renderer);
	void render(SDL_Renderer* sdlRenderer, const Camera& camera);
};

extern std::unique_ptr<Renderer> renderer;