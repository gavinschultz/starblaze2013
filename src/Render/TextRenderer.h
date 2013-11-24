#pragma once
#include <string>
#include <memory>
#include <vector>
#include "Renderer.h"
#include "Phys.h"

class Renderer;

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
	TextRenderer(Renderer* renderer);
	void RenderChar(SDL_Renderer* sdl_renderer, char letter, Point2Di pos, bool red = false) const;
	void RenderString(SDL_Renderer* sdl_renderer, const std::string& text, Point2Di pos, bool red = false) const;
	void RenderPlate(SDL_Renderer* sdl_renderer, const TextPlate& plate) const;
private:
	const uint32_t _scaling;
	const uint32_t _spacing{ 0 };	// character spacing in unscaled pixels
	static const std::unordered_map<char, Point2Di> _char_offsets;
	SDL_Rect _charmap_texture_rect;
	SDL_Texture* _charmap_texture;
};