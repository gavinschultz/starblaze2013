#include "stdafx.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "SpriteLoader.h"

class TextRenderer::impl
{
public:
	SpriteTexture charmap_texture;
};

const std::unordered_map<char, Point2Di> TextRenderer::_char_offsets{
	{ 'A', { 0, 0 } }, { 'B', { 0x10, 0 } }, { 'C', { 0x20, 0 } }, { 'D', { 0x30, 0 } },
	{ 'E', { 0, 010 } }, { 'F', { 0x10, 010 } }, { 'G', { 0x20, 010 } }, { 'H', { 0x30, 010 } },
	{ 'I', { 0, 020 } }, { 'J', { 0x10, 020 } }, { 'K', { 0x20, 020 } }, { 'L', { 0x30, 020 } },
	{ 'M', { 0, 030 } }, { 'N', { 0x10, 030 } }, { 'O', { 0x20, 030 } }, { 'P', { 0x30, 030 } },
	{ 'Q', { 0, 040 } }, { 'R', { 0x10, 040 } }, { 'S', { 0x20, 040 } }, { 'T', { 0x30, 040 } },
	{ 'U', { 0, 050 } }, { 'V', { 0x10, 050 } }, { 'W', { 0x20, 050 } }, { 'X', { 0x30, 050 } },
	{ 'Y', { 0, 060 } }, { 'Z', { 0x10, 060 } }, { '0', { 0x20, 060 } }, { '1', { 0x30, 060 } },
	{ '2', { 0, 070 } }, { '3', { 0x10, 070 } }, { '4', { 0x20, 070 } }, { '5', { 0x30, 070 } },
	{ '6', { 0, 0100 } }, { '7', { 0x10, 0100 } }, { '8', { 0x20, 0100 } }, { '9', { 0x30, 0100 } },
	{ '?', { 0, 0110 } }
};

TextRenderer::TextRenderer(Renderer* renderer, const SpriteTexture& characters_texture) : _scaling{ renderer->scaling }, pimpl{ new impl{} }
{
	pimpl->charmap_texture = characters_texture;
}

TextRenderer::~TextRenderer() {}

void TextRenderer::RenderChar(SDL_Renderer* sdl_renderer, char c, Point2Di pos, CoCoPaletteEnum palette_color) const
{
	if (c >= 'a' && c <= 'z')
		c -= 0x20;
	else if (c == ' ')
		return;

	try
	{
		const SDL_Color& color = renderer->palette->colors[palette_color];
		SDL_SetTextureColorMod(pimpl->charmap_texture.texture, color.r, color.g, color.b);
		const Point2Di& char_offset = _char_offsets.at(c);
		const SDL_Rect src_rect{ pimpl->charmap_texture.rect.x + char_offset.x, pimpl->charmap_texture.rect.y + char_offset.y, 16, 8 };
		const SDL_Rect dst_rect{ pos.x, pos.y, 16 * _scaling, 8 * _scaling };
		SDL_RenderCopy(sdl_renderer, pimpl->charmap_texture.texture, &src_rect, &dst_rect);

		SDL_SetTextureColorMod(pimpl->charmap_texture.texture, 255, 255, 255); // shared texture, so reset to defaults
	}
	catch (std::out_of_range)
	{
		console_debug({ "Could not find texture offset for character '", std::to_string(c), "'" });
		return;
	}
}

void TextRenderer::RenderString(SDL_Renderer* sdl_renderer, const std::string& text, Point2Di pos, CoCoPaletteEnum palette_color) const
{
	for (auto c : text)
	{
		this->RenderChar(sdl_renderer, c, { pos.x, pos.y }, palette_color);
		pos.x += (16 + _spacing) * _scaling;
	}
}

void TextRenderer::RenderPlate(SDL_Renderer* sdl_renderer, const TextPlate& plate) const
{
	for (auto& line : plate.lines)
	{
		this->RenderString(sdl_renderer, line.text, line.offset, CoCoPaletteEnum::yellow);
	}
}

TextLine::TextLine(const std::string& text, Point2Di offset) : text{ text }, offset{ offset } {}

TextPlate::TextPlate(std::initializer_list<TextLine> lines)
{
	this->lines = lines;
}