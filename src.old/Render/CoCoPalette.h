#pragma once
#include <SDL.h>
#include <array>

enum CoCoPaletteEnum
{
	green = 0,
	yellow,
	blue,
	red,
	white,
	cyan,
	magenta,
	orange,
	black,
	darkgreen,
	darkorange
};

namespace CoCoPalette
{
	extern std::array<SDL_Color, 11> colors;
};