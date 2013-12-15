#include "stdafx.h"
#include "mappedtexture.h"
#include "renderutil.h"

SDL_Rect MappedTexture::getScaledRect() const
{
	return SDL_Rect{ rect.x, rect.y, rect.w * scaling_, rect.h * scaling_ };
}