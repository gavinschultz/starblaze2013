#pragma once
#include <memory>
#include "phys.h"
#include "cocopalette.h"

class RenderSystem;
class MappedTexture;
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

class TextRender
{
public:
	TextRender(const RenderSystem& renderer, const MappedTexture& characters_texture);
	~TextRender();
	void RenderChar(SDL_Renderer* sdl_renderer, char letter, Point2Di pos, SDL_Color sdl_color) const;
	void RenderString(SDL_Renderer* sdl_renderer, const std::string& text, Point2Di pos, SDL_Color sdl_color) const;
	void RenderPlate(SDL_Renderer* sdl_renderer, const TextPlate& plate, SDL_Color color) const;
private:
	class impl;	std::unique_ptr<impl> pi;
};