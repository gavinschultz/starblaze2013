#include "stdafx.h"
#include <SDL_ttf.h>
#include "renderdefs.h"
#include "renderutil.h"
#include "timer.h"
#include "phys.h"

#include "spriteloader.h"

class FPSRender::impl
{
public:
	impl(TTF_Font* font, const Window& window);
	TTF_Font* font_;
	mutable int cached_fps_{ 0 };
	Point2Di pos_;
	void render(SDL_Renderer* sdl_renderer, double fps) const;
};

FPSRender::impl::impl(TTF_Font* font, const Window& window) : font_{ font }
{
	int text_w, text_h;
	if (TTF_SizeText(font_, (std::string{ "FPS: 1000" }).c_str(), &text_w, &text_h) < 0)
	{
		debug::console({ "Unable to calculate font size for debug renderer: ", SDL_GetError() });
		return;
	}
	pos_ = { (int)window.w - text_w, 20 };
}

void FPSRender::impl::render(SDL_Renderer* sdl_renderer, double fps) const
{
	if (timer->getTotalFrames() % 10 == 0 || cached_fps_ == 0)
		cached_fps_ = std::lround(fps);

	render::renderSystemText_Bitmap(sdl_renderer, "FPS:" + std::to_string(cached_fps_), pos_.x, pos_.y);
}

FPSRender::FPSRender(TTF_Font* font, const Window& window) : pi{ new impl{ font, window } } {}
FPSRender::~FPSRender() = default;

void FPSRender::render(SDL_Renderer* sdl_renderer, double fps) const
{
	pi->render(sdl_renderer, fps);
}