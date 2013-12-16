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

// Defines a block of game text (e.g. multiple lines) that can be displayed on-screen
class TextPlate
{
public:
	TextPlate(std::initializer_list<TextLine> lines);
	std::vector<TextLine> lines;
};

// This is basically a simplified bitmap font renderer. The bmfontloader.h/.cpp files are slightly more sophisticated and designed for bitmap fonts generated with the program
// BMFont. This text renderer is currently designed just to service the hand-built character map made for this game.
class TextRender
{
public:
	TextRender(const RenderSystem& renderer, const MappedTexture& characters_texture);
	~TextRender();
	void renderChar(SDL_Renderer* sdl_renderer, char letter, Point2Di pos, SDL_Color sdl_color) const;
	void renderString(SDL_Renderer* sdl_renderer, const std::string& text, Point2Di pos, SDL_Color sdl_color) const;
	void renderPlate(SDL_Renderer* sdl_renderer, const TextPlate& plate, SDL_Color color) const;
private:
	class impl;	std::unique_ptr<impl> pi;
};