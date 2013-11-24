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

Renderer::Renderer(unsigned int screen_width, unsigned int screen_height, unsigned int scaling, double world_width) : scaling(scaling), width((unsigned int)(world_width*scaling))
{
	this->window.w = screen_width;
	this->window.h = screen_height;
	init();
}

Renderer::~Renderer()
{
	TTF_CloseFont(_font);
	TTF_Quit();
}

void Renderer::init()
{
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	sdlWindow = SDL_CreateWindow("Starblaze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.w, window.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
	if (!sdlWindow)
	{
		console_debug({ SDL_GetError() });
		exit(4);
	}
	
	toggleFullscreen(is_fullscreen);
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!sdlRenderer)
	{
		console_debug({ SDL_GetError() });
		exit(5);
	}

	sprite_loader.load(sdlRenderer, "resources\\spritesheet.json");

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	SDL_RendererInfo renderer_info;
	SDL_GetRendererInfo(sdlRenderer, &renderer_info);
	console_debug({ "Flags: ", std::to_string(renderer_info.flags), "\nName: ", renderer_info.name });

	SDL_RenderSetLogicalSize(sdlRenderer, 1024, 768);

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
	_text_renderer = std::unique_ptr<TextRenderer>{new TextRenderer{ this }};
}

void Renderer::toggleFullscreen(bool state)
{
	is_fullscreen = state;
	if (state)
		SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(sdlWindow, 0);
}

void Renderer::toggleGrid(bool state)
{
	is_grid_visible = state;
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
	SDL_SetRenderDrawColor(sdlRenderer, 0, 31, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdlRenderer);

	for (auto& sprite : sprite_register.getSprites())
	{
		sprite->render(this->sdlRenderer, *camera);
	}

	renderHUD();

	if (_text_plate)
	{
		_text_renderer->RenderPlate(sdlRenderer, *_text_plate);
		_text_plate.reset();	// release ownership when done
	}

	if (is_grid_visible)
	{
		renderGrid();
		renderFPS((int)timer->getFrameRate());
		renderDebug(*debug.get());
		renderZeroLine(*camera);
	}

	if (is_motionhistory_visible)
		renderMotionHistory(*debug.get());

	//SDL_GL_SwapWindow(this->sdlWindow);
	SDL_RenderPresent(sdlRenderer);
}

void Renderer::renderZeroLine(const Camera& camera)
{
	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	int view_rect_abs_x;
	if (camera.view_rect.x < 0)
		view_rect_abs_x = camera.view_rect.x + this->width;
	else
		view_rect_abs_x = camera.view_rect.x;
	int zero_point_x = this->width - view_rect_abs_x;
	SDL_RenderDrawLine(sdlRenderer, zero_point_x, 0, zero_point_x, window.h);
}

void Renderer::renderGrid()
{

	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
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
		SDL_RenderDrawLine(sdlRenderer, x, y1, x, y1 + line_height);
	}

	glLineWidth(1.0f);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x0101);
	for (uint32_t x = 0; x < this->window.w; x += 256)
	{
		SDL_RenderDrawLine(sdlRenderer, x, 0, x, window.h);
	}
	SDL_RenderDrawLine(sdlRenderer, window.w - 1, 0, window.w - 1, window.h);

	for (uint32_t y = 0; y <= this->window.h; y += 256)
	{
		SDL_RenderDrawLine(sdlRenderer, 0, y, window.w - 1, y);
	}
	SDL_RenderDrawLine(sdlRenderer, 0, window.h - 1, window.w, window.h - 1);

	SDL_SetRenderDrawColor(sdlRenderer, 64, 64, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(sdlRenderer, 48 * scaling, 0, 48 * scaling, window.h);
	SDL_RenderDrawLine(sdlRenderer, 208 * scaling, 0, 208 * scaling, window.h);
	glDisable(GL_LINE_STIPPLE);
}

void Renderer::renderText(const std::string text, uint32_t x, uint32_t y)
{
	SDL_Color text_color = { 0, 0, 0 };
	SDL_Surface* text_surface = TTF_RenderText_Blended(_font, text.c_str(), text_color);
	SDL_Rect text_texture_rect = { 0, 0, text_surface->w, text_surface->h };
	SDL_Rect text_rect = { x, y, text_surface->w, text_surface->h };
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(sdlRenderer, text_surface);
	SDL_FreeSurface(text_surface);
	SDL_RenderCopy(sdlRenderer, text_texture, &text_texture_rect, &text_rect);
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

//void Renderer::renderHUD(const Ship& ship, const Game& game)
void Renderer::renderHUD()
{
	// Fuel, bullets, shields, radar
	SDL_Rect fuel_rect = { 4 * scaling, 157 * scaling, 8 * scaling, 24 * scaling };
	SDL_Rect bullets_rect = { 20 * scaling, 157 * scaling, 8 * scaling, 24 * scaling };
	SDL_Rect shields_rect = { 36 * scaling, 157 * scaling, 8 * scaling, 24 * scaling };
	SDL_Rect radar_rect = { 52 * scaling, 157 * scaling, 8 * scaling, 24 * scaling };
	std::array<SDL_Rect, 4> gauge_rects = { { fuel_rect, bullets_rect, shields_rect, radar_rect } };
	SDL_Color gauge_color = coco_palette.getColor(CoCoPaletteEnum::blue);
	SDL_SetRenderDrawColor(sdlRenderer, gauge_color.r, gauge_color.g, gauge_color.b, 255);
	SDL_RenderFillRects(sdlRenderer, gauge_rects.data(), 4);

	_text_renderer->RenderChar(sdlRenderer, 'F', { 2 * (int32_t)scaling, 184 * (int32_t)scaling }, true);
	_text_renderer->RenderChar(sdlRenderer, 'T', { 18 * (int32_t)scaling, 184 * (int32_t)scaling }, true);
	_text_renderer->RenderChar(sdlRenderer, 'S', { 34 * (int32_t)scaling, 184 * (int32_t)scaling }, true);
	_text_renderer->RenderChar(sdlRenderer, 'R', { 50 * (int32_t)scaling, 184 * (int32_t)scaling }, true);

	// Ships remaining

	// Score

}

void Renderer::renderMotionHistory(const Debug& debug)
{
	const int initial_y = (window.h / 2);
	const int threshold_line_offset = 75;
	const int axis_line_offset = 100;

	glLineWidth(1.0f);

	// background
	SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 176);
	auto bgrect = SDL_Rect{ 0, initial_y - axis_line_offset - threshold_line_offset - 50, window.w, (axis_line_offset + threshold_line_offset + 50) * 2 };
	SDL_RenderFillRect(sdlRenderer, &bgrect);

	// base lines
	SDL_SetRenderDrawColor(sdlRenderer, 128, 128, 128, 255);
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y - axis_line_offset, window.w, initial_y - axis_line_offset);
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y + axis_line_offset, window.w, initial_y + axis_line_offset);

	// threshold lines
	SDL_SetRenderDrawColor(sdlRenderer, 64, 64, 64, 255);
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y - axis_line_offset - threshold_line_offset, window.w, initial_y - axis_line_offset - threshold_line_offset); // X motion - upper threshold
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y + axis_line_offset - threshold_line_offset, window.w, initial_y + axis_line_offset - threshold_line_offset); // X motion - lower threshold
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y - axis_line_offset + threshold_line_offset, window.w, initial_y - axis_line_offset + threshold_line_offset); // Y motion - upper threshold
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y + axis_line_offset + threshold_line_offset, window.w, initial_y + axis_line_offset + threshold_line_offset); // Y motion - lower threshold

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

			SDL_SetRenderDrawColor(sdlRenderer, r, g, b, 255);
			SDL_RenderDrawLine(sdlRenderer, i - 1, prev_render_point_y.x, i, render_point_y.x);
			if (std::abs(scaled_xmotion) > threshold_line_offset)
			{
				SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(sdlRenderer, i, initial_y - axis_line_offset - (threshold_line_offset*util::getsign(scaled_xmotion)));
			}

			SDL_SetRenderDrawColor(sdlRenderer, r, g, b, 255);
			SDL_RenderDrawLine(sdlRenderer, i - 1, prev_render_point_y.y, i, render_point_y.y);
			if (std::abs(scaled_ymotion) > threshold_line_offset)
			{
				SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(sdlRenderer, i, initial_y + axis_line_offset + (threshold_line_offset*util::getsign(scaled_ymotion)));
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
	SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlSurface);
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