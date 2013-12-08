#include "stdafx.h"
#include <array>
#include "sdlutil.h"

std::string sdlutil::getPixelType(Uint32 sdl_pixel_format)
{
	static const std::array<std::string, 12> pixel_types = {
		"SDL_PIXELTYPE_UNKNOWN",
		"SDL_PIXELTYPE_INDEX1",
		"SDL_PIXELTYPE_INDEX4",
		"SDL_PIXELTYPE_INDEX8",
		"SDL_PIXELTYPE_PACKED8",
		"SDL_PIXELTYPE_PACKED16",
		"SDL_PIXELTYPE_PACKED32",
		"SDL_PIXELTYPE_ARRAYU8",
		"SDL_PIXELTYPE_ARRAYU16",
		"SDL_PIXELTYPE_ARRAYU32",
		"SDL_PIXELTYPE_ARRAYF16",
		"SDL_PIXELTYPE_ARRAYF32"
	};

	Uint32 pixel_type = SDL_PIXELTYPE(sdl_pixel_format);
	if (pixel_type >= pixel_types.size())
		return "Unknown (" + std::to_string(pixel_type) + ")";
	return pixel_types[pixel_type];
}

std::string sdlutil::getPixelOrder(Uint32 sdl_pixel_format)
{
	static const std::array<std::string, 3> bitmap_orders = {
		"SDL_BITMAPORDER_NONE",
		"SDL_BITMAPORDER_4321",
		"SDL_BITMAPORDER_1234"
	};

	static const std::array<std::string, 9> packed_orders = {
		"SDL_PACKEDORDER_NONE",
		"SDL_PACKEDORDER_XRGB",
		"SDL_PACKEDORDER_RGBX",
		"SDL_PACKEDORDER_ARGB",
		"SDL_PACKEDORDER_RGBA",
		"SDL_PACKEDORDER_XBGR",
		"SDL_PACKEDORDER_BGRX",
		"SDL_PACKEDORDER_ABGR",
		"SDL_PACKEDORDER_BGRA"
	};

	static const std::array<std::string, 7> array_orders = {
		"SDL_ARRAYORDER_NONE",
		"SDL_ARRAYORDER_RGB",
		"SDL_ARRAYORDER_RGBA",
		"SDL_ARRAYORDER_ARGB",
		"SDL_ARRAYORDER_BGR",
		"SDL_ARRAYORDER_BGRA",
		"SDL_ARRAYORDER_ABGR"
	};

	Uint32 pixel_type = SDL_PIXELTYPE(sdl_pixel_format);
	Uint32 pixel_order = SDL_PIXELORDER(pixel_type);
	try
	{
		if (pixel_type == SDL_PIXELTYPE_INDEX1 || pixel_type == SDL_PIXELTYPE_INDEX4 || pixel_type == SDL_PIXELTYPE_INDEX8)
		{
			return bitmap_orders[pixel_order];
		}
		else if (pixel_type == SDL_PIXELTYPE_PACKED8 || pixel_type == SDL_PIXELTYPE_PACKED16 || pixel_type == SDL_PIXELTYPE_PACKED32)
		{
			return packed_orders[pixel_order];
		}
		else if (pixel_type == SDL_PIXELTYPE_ARRAYU8 || pixel_type == SDL_PIXELTYPE_ARRAYU16 || pixel_type == SDL_PIXELTYPE_ARRAYU32 || pixel_type == SDL_PIXELTYPE_ARRAYF16 || pixel_type == SDL_PIXELTYPE_ARRAYF32)
		{
			return array_orders[pixel_order];
		}
		else
		{
			return "Unknown (" + std::to_string(pixel_order) + ")";
		}
	}
	catch (std::out_of_range e)
	{
		return "Unknown (" + std::to_string(pixel_order) + ") (out-of-range exception: " + e.what() + ")";
	}

}

std::string sdlutil::getPixelLayout(Uint32 sdl_pixel_format)
{
	static const std::array<std::string, 9> pixel_layouts = {
		"SDL_PACKEDLAYOUT_NONE",
		"SDL_PACKEDLAYOUT_332",
		"SDL_PACKEDLAYOUT_4444",
		"SDL_PACKEDLAYOUT_1555",
		"SDL_PACKEDLAYOUT_5551",
		"SDL_PACKEDLAYOUT_565",
		"SDL_PACKEDLAYOUT_8888",
		"SDL_PACKEDLAYOUT_2101010",
		"SDL_PACKEDLAYOUT_1010102"
	};

	Uint32 pixel_type = SDL_PIXELTYPE(sdl_pixel_format);
	Uint32 pixel_layout = SDL_PIXELLAYOUT(pixel_type);
	if (pixel_layout >= pixel_layouts.size())
		return "Unknown (" + std::to_string(pixel_layout) + ")";
	return pixel_layouts[pixel_layout];
}