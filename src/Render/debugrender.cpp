#include "stdafx.h"
#include <algorithm>
#include "renderdefs.h"
#include "renderutil.h"
#include "program.h"

class DebugRender::impl
{
public:
	impl(TTF_Font* font) : font_{ font }
	{
		this->font_height = TTF_FontHeight(font_);
	}
	TTF_Font* font_;
	int font_height;
};

DebugRender::DebugRender(TTF_Font* font) : pi{ new impl{ font } } {}
DebugRender::~DebugRender() = default;

void DebugRender::render(SDL_Renderer* sdl_renderer, const std::unordered_map<std::string, std::string>& debug_items)
{
	//TODO: probably horribly inefficient
	static const int initial_y = 40;
	int y = initial_y;
	int text_w, text_h;
	int max_text_w = 0;
	for (auto& i : debug_items)
	{
		std::string text = i.first + ": ";
		render::renderSystemText_Bitmap(sdl_renderer, text, 20, y);
		//render::renderSystemText_TTF(sdl_renderer, pi->font_, text, 20, y);
		if (TTF_SizeText(pi->font_, text.c_str(), &text_w, &text_h) < 0)
		{
			debug::console({ "Unable to calculate font size: ", SDL_GetError() });
			return;
		}
		max_text_w = std::max(max_text_w, text_w);
		y += pi->font_height + 4;
	}

	y = initial_y;
	for (auto& i : debug_items)
	{
		render::renderSystemText_Bitmap(sdl_renderer, i.second, max_text_w + 50, y);
		//render::renderSystemText_TTF(sdl_renderer, pi->font_, i.value, max_text_w + 50, y);
		y += pi->font_height + 4;
	}
}