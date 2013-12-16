#include "stdafx.h"
#include <unordered_map>
#include "textrender.h"
#include "mappedtexture.h"
#include "debug.h"

class TextRender::impl
{
public:
	impl(const MappedTexture& characters_texture) : charmap_texture{ characters_texture } {}
	MappedTexture charmap_texture;
	const uint32_t spacing{ 0 };	// character spacing in unscaled pixels
	static const std::unordered_map<char, Point2Di> char_offsets;
};

const std::unordered_map<char, Point2Di> TextRender::impl::char_offsets{
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

TextRender::TextRender(const RenderSystem& renderer, const MappedTexture& characters_texture) : pi{ new impl{ characters_texture } } {}
TextRender::~TextRender() {}

void TextRender::renderChar(SDL_Renderer* sdl_renderer, char c, Point2Di pos, SDL_Color color) const
{
	if (c >= 'a' && c <= 'z')
		c -= 0x20;
	else if (c == ' ')
		return;

	try
	{
		SDL_SetTextureColorMod(pi->charmap_texture.texture, color.r, color.g, color.b);
		const Point2Di& char_offset = pi->char_offsets.at(c);
		const SDL_Rect src_rect{ pi->charmap_texture.rect.x + char_offset.x, pi->charmap_texture.rect.y + char_offset.y, 16, 8 };
		const SDL_Rect dst_rect{ pos.x, pos.y, 64, 32 };
		SDL_RenderCopy(sdl_renderer, pi->charmap_texture.texture, &src_rect, &dst_rect);

		SDL_SetTextureColorMod(pi->charmap_texture.texture, 255, 255, 255); // shared texture, so reset to defaults
	}
	catch (std::out_of_range)
	{
		debug::console({ "Could not find texture offset for character '", std::to_string(c), "'" });
		return;
	}
}

void TextRender::renderString(SDL_Renderer* sdl_renderer, const std::string& text, Point2Di pos, SDL_Color color) const
{
	for (auto c : text)
	{
		this->renderChar(sdl_renderer, c, { pos.x, pos.y }, color);
		pos.x += (16 + pi->spacing);
	}
}

void TextRender::renderPlate(SDL_Renderer* sdl_renderer, const TextPlate& plate, SDL_Color color) const
{
	for (auto& line : plate.lines)
	{
		this->renderString(sdl_renderer, line.text, line.offset, color);
	}
}

TextLine::TextLine(const std::string& text, Point2Di offset) : text{ text }, offset{ offset } {}

TextPlate::TextPlate(std::initializer_list<TextLine> lines)
{
	this->lines = lines;
}