#include "stdafx.h"
#include <algorithm>
#include "renderdefs.h"
#include "renderutil.h"
#include "program.h"

class DebugRender::impl
{
public:
	impl(TTF_Font* font)
	{
		font_ = font;
	}
	TTF_Font* font_;
};

DebugRender::DebugRender(TTF_Font* font) : pi{ new impl{ font } } {}
DebugRender::~DebugRender() = default;

void DebugRender::render(SDL_Renderer* sdl_renderer, const std::vector<debug::DebugItem>& debug_items)
{
	//TODO: probably horribly inefficient
	const int initial_y = 20;
	int y = initial_y;
	int text_w, text_h;
	int max_text_w = 0;
	int font_height = TTF_FontHeight(pi->font_);
	for (auto& i : debug_items)
	{
		std::string text = i.label + ": ";
		render::renderSystemText_Bitmap(sdl_renderer, text, 20, y);
		if (TTF_SizeText(pi->font_, text.c_str(), &text_w, &text_h) < 0)
		{
			debug::console({ "Unable to calculate font size: ", SDL_GetError() });
			return;
		}
		max_text_w = std::max(max_text_w, text_w);
		y += font_height + 4;
	}

	y = initial_y;
	for (auto& i : debug_items)
	{
		render::renderSystemText_Bitmap(sdl_renderer, i.value, max_text_w + 50, y);
		y += font_height + 4;
	}
}