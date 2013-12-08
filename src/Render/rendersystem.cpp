#include "stdafx.h"
#include <memory>
#include <sstream>
#include <vector>
#include <SDL_ttf.h>
#include <glew.h>
#include "sdlutil.h"
#include "rendersystem.h"
#include "program.h"
#include "cocopalette.h"
#include "timer.h"
#include "spriteloader.h"

class Window
{
public:
	unsigned int w;
	unsigned int h;
};

class RenderSystem::impl
{
public:
	Window window;
	SDL_Window* sdl_window;
	SDL_Renderer* sdl_renderer;
	SDL_Palette* sdl_palette;
	
	std::unique_ptr<SpriteLoader> sprite_loader;

	bool is_fullscreen{ false };

	TTF_Font* debug_font;

	void renderText(const std::string text, uint32_t x, uint32_t y) const;
	void renderFPS(int fps) const;
	void renderDebug(const std::vector<debug::DebugItem>& items) const;
};

RenderSystem::RenderSystem(unsigned int window_width, unsigned int window_height, unsigned int sprite_scale, int render_width) : pi{ new impl{} }
{
	debug::console({ "Initializing RenderSystem..." });

	pi->window.w = window_width;
	pi->window.h = window_height;
	pi->sprite_loader = std::make_unique<SpriteLoader>();

	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	if (!(pi->sdl_window = SDL_CreateWindow("unnamed", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pi->window.w, pi->window.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL)))
	{
		program::exit(RetCode::sdl_error, { SDL_GetError() });
	}

	setFullscreen(false);
	if (!(pi->sdl_renderer = SDL_CreateRenderer(pi->sdl_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED)))
	{
		program::exit(RetCode::sdl_error, { SDL_GetError() });
	}

	pi->sprite_loader->load(pi->sdl_renderer, "resources\\spritesheet.json");

	int logical_w = 1366;
	int logical_h = 768;
	if (SDL_RenderSetLogicalSize(pi->sdl_renderer, logical_w, logical_h))
	{
		program::exit(RetCode::sdl_error, { "Error setting logical size (", std::to_string(logical_w), ",", std::to_string(logical_h), ")", SDL_GetError() });
	}

	GLenum glew_init = glewInit();
	if (glew_init != GLEW_OK)
	{
		std::string glew_error = (char*)glewGetErrorString(glew_init);
		program::exit(RetCode::runtime_error, { "GLEW initialization error: ", glew_error });
	}

	if (TTF_Init() == -1) 
	{
		program::exit(RetCode::sdl_error, { "Error initializing SDL_TTF: %s\n", TTF_GetError() });
	}

	if (!(pi->debug_font = TTF_OpenFont("resources\\DroidSans.ttf", 16))) 
	{
		program::exit(RetCode::sdl_error, { "Error opening debug font: %s\n", TTF_GetError() });
	}

	pi->sdl_palette = SDL_AllocPalette(11);
	SDL_SetPaletteColors(pi->sdl_palette, CoCoPalette::colors.data(), 0, 11);

	//_text_renderer = std::unique_ptr<TextRenderer>{new TextRenderer{ this, sprite_loader.getSprite("characters") }};

	debug::console({ "Renderer info\n--------------\n", getInfo() });
}
RenderSystem::~RenderSystem() {}

void RenderSystem::update()
{
	SDL_RenderClear(pi->sdl_renderer);
	SDL_SetRenderDrawColor(pi->sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	
	pi->renderFPS(std::lround(timer->getFrameRate()));
	pi->renderDebug(debug::getItems());

	SDL_RenderPresent(pi->sdl_renderer);
}

void RenderSystem::impl::renderText(const std::string text, uint32_t x, uint32_t y) const
{
	SDL_Color text_color = { 255, 255, 255 };
	SDL_Surface* text_surface = TTF_RenderText_Blended(debug_font, text.c_str(), text_color);
	if (!text_surface)
	{
		debug::console({ "Unable to initialize font (mostly for debugging): ", SDL_GetError() });
		return;
	}
	SDL_Rect text_texture_rect = { 0, 0, text_surface->w, text_surface->h };
	SDL_Rect text_rect = { x, y, text_surface->w, text_surface->h };
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(sdl_renderer, text_surface);
	if (!text_texture)
	{
		debug::console({ "Unable to create texture from surface: ", SDL_GetError() });
		return;
	}
	SDL_FreeSurface(text_surface);
	if (SDL_RenderCopy(sdl_renderer, text_texture, &text_texture_rect, &text_rect) < 0)
	{
		debug::console({ "Unable to render text texture: ", SDL_GetError() });
		return;
	}
	SDL_DestroyTexture(text_texture);
}

void RenderSystem::impl::renderFPS(int fps) const
{
	static int cached_fps;
	if (timer->getTotalFrames() % 10 == 0)
		cached_fps = fps;
	this->renderText("FPS:" + std::to_string(fps), 50, 50);
}

void RenderSystem::impl::renderDebug(const std::vector<debug::DebugItem>& items) const
{
	//TODO: probably horribly inefficient

	const int initial_y = 100;
	int y = initial_y;
	int text_w, text_h;
	int max_text_w = 0;
	int font_height = TTF_FontHeight(debug_font);
	for (auto& i : items)
	{
		std::string text = i.label + ": ";
		this->renderText(text, 50, y);
		if (TTF_SizeText(debug_font, text.c_str(), &text_w, &text_h) < 0)
		{
			debug::console({ "Unable to calculate font size: ", SDL_GetError() });
			return;
		}
		max_text_w = std::max(max_text_w, text_w);
		y += font_height + 4;
	}

	y = initial_y;
	for (auto& i : items)
	{
		this->renderText(i.value, max_text_w + 50, y);
		y += font_height + 4;
	}
}

bool RenderSystem::isFullscreen() const 
{ 
	return pi->is_fullscreen; 
}

void RenderSystem::setFullscreen(bool state)
{
	pi->is_fullscreen = state;
	int flag;
	if (state)
		flag = SDL_WindowFlags::SDL_WINDOW_FULLSCREEN_DESKTOP;
	else
		flag = 0;

	if (SDL_SetWindowFullscreen(pi->sdl_window, flag))
	{
		debug::console({ "Error setting fullscreen to ", std::to_string(state), ": ", SDL_GetError() });
	}
}

const SpriteLoader& RenderSystem::getSpriteLoader() const
{
	return *(pi->sprite_loader);
}

std::string RenderSystem::getInfo() const
{
	std::stringstream ss;
	SDL_RendererInfo renderer_info;
	if (SDL_GetRendererInfo(pi->sdl_renderer, &renderer_info))
	{
		debug::console({ "Error getting SDL renderer info: ", SDL_GetError() });
		return "Error getting SDL renderer info";
	}

	int logical_w, logical_h;
	SDL_RenderGetLogicalSize(pi->sdl_renderer, &logical_w, &logical_h);

	ss << "Renderer name:        " << renderer_info.name << "\n";
	ss << "Logical window size:  " << "(" << logical_w << "," << logical_h << ")\n";
	ss << "Actual window size:   " << "(" << pi->window.w << "," << pi->window.h << ")\n";
	ss << "OpenGL version:       " << glGetString(GL_VERSION) << "\n";
	ss << "Is fullscreen:        " << (isFullscreen() ? "yes" : "no") << "\n";
	ss << "Max texture height:   " << renderer_info.max_texture_height << "\n";
	ss << "Max texture width:    " << renderer_info.max_texture_width << "\n";
	ss << "Hardware accelerated: " << ((renderer_info.flags & SDL_RendererFlags::SDL_RENDERER_ACCELERATED) ? "yes" : "no") << "\n";
	ss << "Software fallback:    " << ((renderer_info.flags & SDL_RendererFlags::SDL_RENDERER_SOFTWARE) ? "yes" : "no") << "\n";
	ss << "VSYNC enabled:        " << ((renderer_info.flags & SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC) ? "yes" : "no") << "\n";
	ss << "Render to texture:    " << ((renderer_info.flags & SDL_RendererFlags::SDL_RENDERER_TARGETTEXTURE) ? "yes" : "no") << "\n";

	for (unsigned int i = 0; i < renderer_info.num_texture_formats; i++)
	{
		Uint32 texture_format = renderer_info.texture_formats[i];
		ss << "Texture format " << i << ":     " << SDL_GetPixelFormatName(texture_format) << "\n";
		ss << "  Pixel type:         " << sdlutil::getPixelType(texture_format) << "\n";
		ss << "  Pixel order:        " << sdlutil::getPixelOrder(texture_format) << "\n";
		ss << "  Pixel layout:       " << sdlutil::getPixelLayout(texture_format) << "\n";
		ss << "  Bits per pixel:     " << SDL_BITSPERPIXEL(texture_format) << "\n";
		ss << "  Bytes per pixel:    " << SDL_BYTESPERPIXEL(texture_format) << "\n";
		ss << "  Has palette:        " << (SDL_ISPIXELFORMAT_INDEXED(texture_format) ? "yes" : "no") << "\n";
		ss << "  Has alpha:          " << (SDL_ISPIXELFORMAT_ALPHA(texture_format) ? "yes" : "no") << "\n";
		//ss << "  Special format:     " << (SDL_ISPIXELFORMAT_FOURCC(texture_format) ? "yes" : "no") << "\n";	// doesn't seem important...
	}

	return ss.str();
}

extern std::unique_ptr<RenderSystem> rendersys;