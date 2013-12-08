#pragma once
#include "Phys.h"
#include "CoCoPalette.h"

class Renderer;
class SpriteTexture;
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
	TextRenderer(Renderer* renderer, const SpriteTexture& characters_texture);
	~TextRenderer();
	void RenderChar(SDL_Renderer* sdl_renderer, char letter, Point2Di pos, CoCoPaletteEnum palette_color) const;
	void RenderString(SDL_Renderer* sdl_renderer, const std::string& text, Point2Di pos, CoCoPaletteEnum palette_color) const;
	void RenderPlate(SDL_Renderer* sdl_renderer, const TextPlate& plate) const;
private:
	class impl;
	std::unique_ptr<impl> pimpl;
	const uint32_t _scaling;
	const uint32_t _spacing{ 0 };	// character spacing in unscaled pixels
	static const std::unordered_map<char, Point2Di> _char_offsets;
};