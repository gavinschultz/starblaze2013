#include "stdafx.h"
#include "renderdefs.h"
#include "rendersystem.h"
#include "renderutil.h"
#include "mappedtexture.h"
#include "spriteloader.h"
#include "playfield.h"

class AlienRender::impl
{
public:
	MappedTexture alien_texture;
};

AlienRender::AlienRender(const RenderSystem& renderer) : pi{ new impl{} }
{
	pi->alien_texture = renderer.getSpriteLoader().getSprite("alien1");
}
AlienRender::~AlienRender() = default;

void AlienRender::render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent& state) const
{
	auto playfield = db->getPlayField();
	SDL_Rect dstrect =
	{
		
		renderutil::getScreenXForEntityByCameraAndDistance(state.interpolated.x, pi->alien_texture.getScaledRect().w, playfield->w, camera, 1.0),
		std::lround(state.interpolated.y - camera.getViewRect().y),
		pi->alien_texture.getScaledRect().w,
		pi->alien_texture.getScaledRect().h
	};

	//if (alien.is_collided)
	//	SDL_SetTextureColorMod(_alien_texture.texture, 0, 0, 0);
	//else
	//	SDL_SetTextureColorMod(_alien_texture.texture, 255, 255, 255);

	SDL_RenderCopy(sdl_renderer, pi->alien_texture.texture, &pi->alien_texture.rect, &dstrect);
	//SDL_SetTextureColorMod(pi->alien_texture.texture, 255, 255, 255); // shared texture, so reset to defaults

	//renderer->renderNormalVector({ dstrect.x + ((alien.bounding_box.w * (int)_scaling) / 2), dstrect.y + ((alien.bounding_box.h * (int)_scaling) / 2) }, alien.attack_vector_n);
}