#pragma once

namespace sdlutil
{
	std::string getPixelType(Uint32 sdl_pixel_format);
	std::string getPixelOrder(Uint32 sdl_pixel_format);
	std::string getPixelLayout(Uint32 sdl_pixel_format);
}