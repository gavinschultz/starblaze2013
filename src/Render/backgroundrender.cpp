#include "stdafx.h"
#include <array>
#include "cocopalette.h"
#include "rendersystem.h"
#include "renderdefs.h"
#include "playfield.h"
#include "renderutil.h"
#include "spriteloader.h"

class BackgroundRender::impl
{
public:
	impl(const RenderSystem& renderer);

	const int HILL_HEIGHT = 4*4;
	const int HILL_WIDTH = 16*4;
	const int SKY_HEIGHT = 96*4;
	const int GROUND_HEIGHT = 48*4;

	std::array<MappedTexture,4> hill_sprite_textures;

	SDL_Rect sky_rect;
	SDL_Color sky_color;

	SDL_Rect ground_rect;
	SDL_Color ground_color;

	SDL_Rect bg_rect;
	SDL_Color bg_color;

	void render(SDL_Renderer* sdl_renderer, const Camera& camera, const PlayField& playfield) const;
};

BackgroundRender::impl::impl(const RenderSystem& renderer)
{
	auto& window = renderer.getWindow();
	sky_rect = { 0, 0, window.w, SKY_HEIGHT };
	sky_color = renderer.palette->colors[CoCoPaletteEnum::blue];
	ground_rect = { 0, sky_rect.h, window.w, GROUND_HEIGHT };
	ground_color = renderer.palette->colors[CoCoPaletteEnum::green];

	bg_rect = { 0, 0, window.w, window.h };
	bg_color = renderer.palette->colors[CoCoPaletteEnum::green];

	hill_sprite_textures[0] = renderer.getSpriteLoader().getSprite("hill1");
	hill_sprite_textures[1] = renderer.getSpriteLoader().getSprite("hill2");
	hill_sprite_textures[2] = renderer.getSpriteLoader().getSprite("hill3");
	hill_sprite_textures[3] = renderer.getSpriteLoader().getSprite("hill4");
}

void BackgroundRender::impl::render(SDL_Renderer* sdl_renderer, const Camera& camera, const PlayField& playfield) const
{
	const std::array<int, 4> y_channel_coords = { { 0 - HILL_HEIGHT, 36, 68, 100 } };
	SDL_SetRenderDrawColor(sdl_renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderFillRect(sdl_renderer, &bg_rect);
	SDL_SetRenderDrawColor(sdl_renderer, sky_color.r, sky_color.g, sky_color.b, sky_color.a);
	SDL_RenderFillRect(sdl_renderer, &sky_rect);
	SDL_SetRenderDrawColor(sdl_renderer, ground_color.r, ground_color.g, ground_color.b, ground_color.a);
	SDL_RenderFillRect(sdl_renderer, &ground_rect);

	for (auto hill : playfield.hills)
	{
		auto hill_sprite_texture = hill_sprite_textures[hill.type];

		if (hill.y_channel == 0)
			SDL_SetTextureColorMod(hill_sprite_texture.texture, ground_color.r, ground_color.g, ground_color.b);
		else
			SDL_SetTextureColorMod(hill_sprite_texture.texture, sky_color.r, sky_color.g, sky_color.b);
		int32_t entity_x_at_camera_x = renderutil::getScreenXForEntityByCameraAndDistance(hill.x, hill_sprite_texture.rect.w, (int)playfield.w, camera, hill.distance_factor);
		SDL_Rect hill_rect = {
			entity_x_at_camera_x,
			SKY_HEIGHT + y_channel_coords[hill.y_channel],
			HILL_WIDTH,
			HILL_HEIGHT
		};
		SDL_RenderCopy(sdl_renderer, hill_sprite_texture.texture, &hill_sprite_texture.rect, &hill_rect);
		SDL_SetTextureColorMod(hill_sprite_texture.texture, 255, 255, 255);
	}
}

BackgroundRender::BackgroundRender(const RenderSystem& renderer) : pi{ new impl{renderer} } {}
BackgroundRender::~BackgroundRender() = default;

void BackgroundRender::render(SDL_Renderer* sdl_renderer, const Camera& camera, const PlayField& playfield) const
{
	pi->render(sdl_renderer, camera, playfield);
}