#include "stdafx.h"
#include <array>
#include "component\stationcomponent.h"
#include "mappedtexture.h"
#include "rendersystem.h"
#include "spriteloader.h"
#include "renderdefs.h"
#include "renderutil.h"
#include "playfield.h"

class StationRender::impl
{
public:
	std::array<MappedTexture, 2> station_textures;
};

StationRender::StationRender(const RenderSystem& renderer) : pi{ new impl{} } 
{
	pi->station_textures[StationType::fuel] = renderer.getSpriteLoader().getSprite("fuel");
	pi->station_textures[StationType::repair] = renderer.getSpriteLoader().getSprite("repair");
}
StationRender::~StationRender() = default;

void StationRender::render(SDL_Renderer* sdl_renderer, const Camera& camera, const StationComponent& station, const TemporalState2DComponent& state, const PhysicalComponent& phys) const
{
	auto playfield = db->getPlayField();
	const auto& texture = pi->station_textures[station.station_type];
	SDL_Rect station_rect =
	{
		renderutil::getScreenXForEntityByCameraAndDistance(state.current.pos.x, texture.rect.w, playfield->w, camera, 1.0),
		playfield->getPlayArea(phys.box).h,
		texture.getScaledRect().w,
		texture.getScaledRect().h
	};

	if (station.is_docked)
		SDL_SetTextureColorMod(texture.texture, 0, 0, 0);
	else
		SDL_SetTextureColorMod(texture.texture, 255, 255, 255);

	SDL_RenderCopy(sdl_renderer, texture.texture, &texture.rect, &station_rect);
}