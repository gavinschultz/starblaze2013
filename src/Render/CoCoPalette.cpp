#include "stdafx.h"
#include "CocoPalette.h"

const SDL_Color& CoCoPalette::getColor(const CoCoPaletteEnum colorEnum) const
{
	if (coco_palette.count(colorEnum) > 0)
		return coco_palette.at(colorEnum);
	else
		return coco_palette.at(black);
}