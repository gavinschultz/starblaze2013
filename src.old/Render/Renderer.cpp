#include "stdafx.h"
#include <glew.h>
#include <SDL_image.h>
#include "Util.h"
#include "CoCoPalette.h"
#include "Renderer.h"
#include "Timer.h"
#include <limits>
#include <array>
#include "SpriteLoader.h"
#include "Game.h"
#include "StationSprite.h"
#include "BGSprite.h"
#include "ShipSprite.h"
#include "AlienSprite.h"
#include "RadarSprite.h"
#include "HUDRend.h"
#include "BulletSprite.h"
#include "Entity\Station.h"
#include "Entity\World.h"
#include "Entity\Alien.h"
#include "Entity\Bullet.h"
#include "Entity\Debris.h"

Renderer::Renderer(unsigned int screen_width, unsigned int screen_height, unsigned int scaling, double world_width) : scaling(scaling), width((unsigned int)(world_width*scaling))
{
	this->window.w = screen_width;
	this->window.h = screen_height;
	init();
}

Renderer::~Renderer()
{
	//SDL_FreePalette(palette);
	//TTF_CloseFont(_font);
	//TTF_Quit();
	//SDL_DestroyRenderer(sdl_renderer);
	//SDL_DestroyWindow(sdl_window);
}

void Renderer::init()
{
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	sdl_window = SDL_CreateWindow("Starblaze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.w, window.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
	if (!sdl_window)
	{
		console_debug({ SDL_GetError() });
		exit(4);
	}

	toggleFullscreen(is_fullscreen);
	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!sdl_renderer)
	{
		console_debug({ SDL_GetError() });
		exit(5);
	}

	sprite_loader.load(sdl_renderer, "resources\\spritesheet.json");

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	SDL_RendererInfo renderer_info;
	SDL_GetRendererInfo(sdl_renderer, &renderer_info);
	console_debug({ "Flags: ", std::to_string(renderer_info.flags), "\nName: ", renderer_info.name });

	SDL_RenderSetLogicalSize(sdl_renderer, 1366, 768);
	//SDL_RenderSetScale(sdl_renderer, 0.25, 0.25);

	GLenum glew_init = glewInit();
	if (GLEW_OK != glew_init)
	{
		// GLEW failed!
		std::string glew_error = (char*)glewGetErrorString(glew_init);
		console_debug({ "GLEW initialization error: ", glew_error });
		exit(1);
	}

	console_debug({ "GL version: ", (char*)glGetString(GL_VERSION) });

	if (TTF_Init() == -1) {
		console_debug({ "TTF_Init: %s\n", TTF_GetError() });
		exit(2);
	}
	_font = TTF_OpenFont("resources\\DroidSans.ttf", 16);
	if (!_font) {
		console_debug({ "TTF_OpenFont: %s\n", TTF_GetError() });
		exit(3);
	}

	palette = SDL_AllocPalette(11);
	SDL_SetPaletteColors(palette, CoCoPalette::colors.data(), 0, 11);

	_text_renderer = std::unique_ptr<TextRenderer>{new TextRenderer{ this, sprite_loader.getSprite("characters") }};
}

void Renderer::toggleFullscreen(bool state)
{
	is_fullscreen = state;
	if (state)
		SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(sdl_window, 0);
}

void Renderer::toggleGrid(bool state)
{
	is_grid_visible = state;
	are_collision_boxes_visible = state;
}

void Renderer::toggleMotionHistory(bool state)
{
	is_motionhistory_visible = state;
}

// only works for absolute coordinates
bool Renderer::isRightOf(int32_t x, int32_t y)
{
	if ((x - y) >= 0)
		return ((x - y) % width) < width / 2;
	else
		return ((y - x) % width) > width / 2;
}

bool Renderer::isLeftOf(int32_t x, int32_t y) {
	return !isRightOf(x, y);
}

void Renderer::render(Camera* camera)
{
	SDL_SetRenderDrawColor(sdl_renderer, 0, 31, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdl_renderer);

	sprite_register.getBackground().render(sdl_renderer, *camera, *world);

	auto station = game->entity_register.getStation();
	if (station)
	{
		const auto& station_sprite = (StationSprite&)sprite_register.getSprite(station);
		station_sprite.render(sdl_renderer, *camera, *station);
	}

	for (auto& alien : game->entity_register.getAliens())
	{
		auto& enemySprite = (AlienSprite&)sprite_register.getSprite((Entity*)alien.get());
		enemySprite.render(sdl_renderer, *camera, *alien);
	}

	auto ship = game->entity_register.getShip();
	if (ship)
	{
		const auto& ship_sprite = (ShipSprite&)sprite_register.getPlayerShip();
		ship_sprite.render(sdl_renderer, *camera, *ship);
	}

	for (auto& debris : game->entity_register.getDebris())
	{
		for (auto& debris_item : debris->getItems())
		{
			if (!debris_item->isAlive())
				continue;

			SDL_Rect debris_item_rect = {	// TODO: use state.interpolated instead of state.current
				renderutil::getScreenXForEntityByCameraAndDistance(debris_item->state.current.pos.x * scaling, 2 * scaling, renderer->width, *camera, 1.0),
				std::lround(debris_item->state.current.pos.y * scaling - camera->view_rect.y),
				2 * scaling,
				2 * scaling
			};
			SDL_SetRenderDrawColor(sdl_renderer, 240, 122, 189, 255);
			SDL_RenderFillRect(sdl_renderer, &debris_item_rect);
		}
	}

	auto& bullets = game->entity_register.getBullets();
	for (auto& bullet : bullets)
	{
		const auto& bullet_sprite = (BulletSprite&)sprite_register.getSprite((Entity*)bullet.get());
		bullet_sprite.render(sdl_renderer, *camera, *bullet);
	}

	auto& hud_rend = sprite_register.getHUD();
	hud_rend.render(sdl_renderer, *(_text_renderer.get()), *ship, 0, 0); // TODO: lives, score

	auto& radar_rend = sprite_register.getRadar();
	radar_rend.render(sdl_renderer, *camera);

	if (_text_plate)
	{
		_text_renderer->RenderPlate(sdl_renderer, *_text_plate);
		_text_plate.reset();	// release ownership when done
	}

	if (is_grid_visible)
	{
		renderGrid();
		renderFPS((int)timer->getFrameRate());
		renderDebug(*debug.get());
		renderZeroLine(*camera);
		renderCollisionBoxes(*camera);
	}

	if (is_motionhistory_visible)
		renderMotionHistory(*debug.get());

	SDL_RenderPresent(sdl_renderer);
}

void Renderer::renderZeroLine(const Camera& camera)
{
	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	int view_rect_abs_x;
	if (camera.view_rect.x < 0)
		view_rect_abs_x = camera.view_rect.x + this->width;
	else
		view_rect_abs_x = camera.view_rect.x;
	int zero_point_x = this->width - view_rect_abs_x;
	SDL_RenderDrawLine(sdl_renderer, zero_point_x, 0, zero_point_x, window.h);
}

void Renderer::renderCollisionBoxes(const Camera& camera)
{
	std::vector<SDL_Rect> transformed_rects;
	std::vector<SDL_Rect> transformed_collided_rects;
	for (auto& entity : game->entity_register.getAll())
	{
		//for (auto& collision_box : entity->getCollidable().getCollisionBoxes())
		//{
		//	SDL_Rect transformed_rect = SDL_Rect{
		//	renderutil::getScreenXForEntityByCameraAndDistance(collision_box.x * scaling, collision_box.w * scaling, this->width, camera, 1.0),
		//	collision_box.y * scaling,
		//	collision_box.w * scaling,
		//	collision_box.h * scaling };
		//	transformed_rects.push_back(transformed_rect);
		//}
	}
	glLineWidth(1.0f);
	SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRects(sdl_renderer, transformed_rects.data(), transformed_rects.size());
}

void Renderer::renderGrid()
{
	glPointSize(1.0f);
	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	for (uint32_t x = 0; x <= this->window.w; x += 8)
	{
		int y1 = 8;
		int line_height = 0;
		if ((x % 64) < 32)
			line_height = 32 - (x % 32);
		else if ((x % 64) == 32)
			line_height = 16;
		else
			line_height = x % 32;
		SDL_RenderDrawLine(sdl_renderer, x, y1, x, y1 + line_height);
	}

	glLineWidth(1.0f);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x0101);
	for (uint32_t x = 0; x < this->window.w; x += 256)
	{
		SDL_RenderDrawLine(sdl_renderer, x, 0, x, window.h);
	}
	SDL_RenderDrawLine(sdl_renderer, window.w - 1, 0, window.w - 1, window.h);

	for (uint32_t y = 0; y <= this->window.h; y += 256)
	{
		SDL_RenderDrawLine(sdl_renderer, 0, y, window.w - 1, y);
	}
	SDL_RenderDrawLine(sdl_renderer, 0, window.h - 1, window.w, window.h - 1);

	SDL_SetRenderDrawColor(sdl_renderer, 64, 64, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(sdl_renderer, world->ship_limits.x * scaling, 0, world->ship_limits.x * scaling, window.h);
	SDL_RenderDrawLine(sdl_renderer, (world->ship_limits.x + world->ship_limits.w) * scaling, 0, (world->ship_limits.x + world->ship_limits.w) * scaling, window.h);
	glDisable(GL_LINE_STIPPLE);
}

void Renderer::renderText(const std::string text, uint32_t x, uint32_t y)
{
	SDL_Color text_color = { 0, 0, 0 };
	SDL_Surface* text_surface = TTF_RenderText_Blended(_font, text.c_str(), text_color);
	SDL_Rect text_texture_rect = { 0, 0, text_surface->w, text_surface->h };
	SDL_Rect text_rect = { x, y, text_surface->w, text_surface->h };
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(sdl_renderer, text_surface);
	SDL_FreeSurface(text_surface);
	SDL_RenderCopy(sdl_renderer, text_texture, &text_texture_rect, &text_rect);
	SDL_DestroyTexture(text_texture);
}

void Renderer::renderFPS(int fps)
{
	static int cached_fps;
	if (timer->getTotalFrames() % 10 == 0)
		cached_fps = fps;
	this->renderText("FPS:" + std::to_string(fps), 50, 50);
}

void Renderer::renderDebug(const Debug& debug)
{
	//TODO: probably horribly inefficient

	const int initial_y = 100;
	int y = initial_y;
	int text_w, text_h;
	int max_text_w = 0;
	int font_height = TTF_FontHeight(_font);
	for (auto& i : debug.items)
	{
		std::string text = i->label + ": ";
		this->renderText(text, 50, y);
		TTF_SizeText(_font, text.c_str(), &text_w, &text_h);
		max_text_w = std::max(max_text_w, text_w);
		y += font_height + 4;
	}

	y = initial_y;
	for (auto& i : debug.items)
	{
		this->renderText(i->value, max_text_w + 50, y);
		y += font_height + 4;
	}
}

void Renderer::renderMotionHistory(const Debug& debug)
{
	const int initial_y = (window.h / 2);
	const int threshold_line_offset = 75;
	const int axis_line_offset = 100;

	glLineWidth(1.0f);

	// background
	SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 176);
	auto bgrect = SDL_Rect{ 0, initial_y - axis_line_offset - threshold_line_offset - 50, window.w, (axis_line_offset + threshold_line_offset + 50) * 2 };
	SDL_RenderFillRect(sdl_renderer, &bgrect);

	// base lines
	SDL_SetRenderDrawColor(sdl_renderer, 128, 128, 128, 255);
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y - axis_line_offset, window.w, initial_y - axis_line_offset);
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y + axis_line_offset, window.w, initial_y + axis_line_offset);

	// threshold lines
	SDL_SetRenderDrawColor(sdl_renderer, 64, 64, 64, 255);
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y - axis_line_offset - threshold_line_offset, window.w, initial_y - axis_line_offset - threshold_line_offset); // X motion - upper threshold
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y + axis_line_offset - threshold_line_offset, window.w, initial_y + axis_line_offset - threshold_line_offset); // X motion - lower threshold
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y - axis_line_offset + threshold_line_offset, window.w, initial_y - axis_line_offset + threshold_line_offset); // Y motion - upper threshold
	SDL_RenderDrawLine(sdl_renderer, 0, initial_y + axis_line_offset + threshold_line_offset, window.w, initial_y + axis_line_offset + threshold_line_offset); // Y motion - lower threshold

	// points
	Vector2Di render_point_y{ 0, 0 };
	Vector2Di prev_render_point_y;
	for (unsigned int i = 0; i < debug.motion_history.size(); i++)
	{
		Vector2D p = debug.motion_history[i];
		glPointSize(1.0f);

		int scaled_xmotion = std::lround(((double)threshold_line_offset / debug.motion_max_thresholds.x) * p.x); // X motion
		int scaled_ymotion = std::lround(((double)threshold_line_offset / debug.motion_max_thresholds.y) * p.y); // Y motion

		if (i > 0)
		{
			prev_render_point_y = render_point_y;
			render_point_y = { initial_y - axis_line_offset - scaled_xmotion, initial_y + axis_line_offset + scaled_ymotion };
			Vector2D prev_p = debug.motion_history[i - 1];

			int r, g, b;
			int frame_age = (int)debug.motion_history_counter - (int)i;
			if (frame_age < 0)
				frame_age += debug.motion_history_limit;
			b = 255 - std::min(frame_age * 2, 255);
			g = 155 + std::min(frame_age * 2, 255 - 155);
			r = 64 + std::min(frame_age, 255 - 64);

			SDL_SetRenderDrawColor(sdl_renderer, r, g, b, 255);
			SDL_RenderDrawLine(sdl_renderer, i - 1, prev_render_point_y.x, i, render_point_y.x);
			if (std::abs(scaled_xmotion) > threshold_line_offset)
			{
				SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(sdl_renderer, i, initial_y - axis_line_offset - (threshold_line_offset*util::getsign(scaled_xmotion)));
			}

			SDL_SetRenderDrawColor(sdl_renderer, r, g, b, 255);
			SDL_RenderDrawLine(sdl_renderer, i - 1, prev_render_point_y.y, i, render_point_y.y);
			if (std::abs(scaled_ymotion) > threshold_line_offset)
			{
				SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(sdl_renderer, i, initial_y + axis_line_offset + (threshold_line_offset*util::getsign(scaled_ymotion)));
			}
		}
	}
}

void Renderer::renderTextPlate(std::shared_ptr<TextPlate> text_plate)
{
	this->_text_plate = text_plate;
}

SDL_Texture* Renderer::loadTextureFromFile(std::string imagePath, SDL_Rect* texture_rect)
{
	console_debug({ "Renderer::loadTextureFromFile is obsolete, replace with renderutils::loadTextureFromFile" });
	SDL_Surface* sdlSurface = IMG_Load(imagePath.c_str());
	if (!sdlSurface)
	{
		console_debug({ SDL_GetError() });
		return nullptr;
	}
	SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(sdl_renderer, sdlSurface);
	SDL_FreeSurface(sdlSurface);
	SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
	if (texture_rect != nullptr)
	{
		texture_rect->x = 0;
		texture_rect->y = 0;
		texture_rect->w = sdlSurface->w;
		texture_rect->h = sdlSurface->h;
	};
	return sdlTexture;
}

void Renderer::renderNormalVector(const Point2Di& src, const Vector2D& v) const
{
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	int dest_x = (int)(src.x + v.x * 32);
	int dest_y = (int)(src.y + v.y * 32);
	SDL_RenderDrawLine(sdl_renderer, src.x, src.y, dest_x, dest_y);
	glPointSize(6.0f);
	SDL_RenderDrawPoint(sdl_renderer, dest_x, dest_y);
}