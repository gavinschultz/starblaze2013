#include "stdafx.h"
#include <array>
#include <algorithm>
#include <memory>
#include "component.h"
#include "rendersystem.h"
#include "renderdefs.h"
#include "renderutil.h"
#include "mappedtexture.h"
#include "spriteloader.h"
#include "cocopalette.h"
#include "prefs.h"
#include "timer.h"
#include "playfield.h"

class ShipRender::impl
{
public:
	SDL_Color taillight_color_;
	SDL_Rect taillight_rect_;
	MappedTexture stripe_texture_;
	MappedTexture wheels_texture_;
	std::array<MappedTexture, 4> burner_textures_;
	MappedTexture ship_texture_;

	Vector2Di stripe_offset_;
	Vector2Di burner_offset_;
	Vector2Di wheels_offset_;

	impl(const RenderSystem& renderer)
	{
		auto& sprite_loader = renderer.getSpriteLoader();
		
		this->stripe_texture_ = sprite_loader.getSprite("shipstripe_line");
		this->wheels_texture_ = sprite_loader.getSprite("shipwheels");
		this->burner_textures_[0] = sprite_loader.getSprite("burner0");
		this->burner_textures_[1] = sprite_loader.getSprite("burner1");
		this->burner_textures_[2] = sprite_loader.getSprite("burner2");
		this->burner_textures_[3] = sprite_loader.getSprite("burner3");
		this->ship_texture_ = sprite_loader.getSprite("ship");

		this->stripe_offset_ = { 0, 24 };
		this->burner_offset_ = { -this->burner_textures_[0].getScaledRect().w, ship_texture_.getScaledRect().h - 20 };
		this->wheels_offset_ = { 0, this->ship_texture_.getScaledRect().h };
		this->taillight_rect_ = { 0, 0, 8, 4 };

		taillight_color_ = renderer.palette->colors[CoCoPaletteEnum::red];
	}
};

ShipRender::ShipRender(const RenderSystem& renderer) : pi{ new impl(renderer) } {}
ShipRender::~ShipRender() = default;
void ShipRender::render(SDL_Renderer* sdl_renderer, const Camera& camera, const TemporalState2DComponent& state, const HorizontalOrientComponent& orient, const ThrustComponent& thrust, const PhysicalComponent& phys, const CollisionComponent* collide) const
{
	auto playfield = db->getPlayField();

	SDL_RendererFlip flip = (orient.direction == HOrient::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	//SDL_RendererFlip flipReverse = (orient.direction == HOrient::left ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

	int32_t entity_x = renderutil::getScreenXForEntityByCameraAndDistance(state.interpolated.x, pi->ship_texture_.getScaledRect().w, (unsigned int)playfield->w, camera, 1.0);
	int32_t entity_y = std::lround(state.interpolated.y - camera.getViewRect().y);
	SDL_Rect ship_rect = { entity_x, entity_y, pi->ship_texture_.getScaledRect().w, pi->ship_texture_.getScaledRect().h };

	SDL_Rect stripe_texture_rect = { pi->stripe_texture_.rect.x, pi->stripe_texture_.rect.y, pi->ship_texture_.rect.w, pi->stripe_texture_.rect.h };
	if (prefs::smooth_animation)
		stripe_texture_rect.x += (timer->getTotalFrames() - 1) % stripe_texture_rect.w;				// smooth scrolling stripe
	else
		stripe_texture_rect.x += (((timer->getTotalFrames() - 1) % stripe_texture_rect.w) / 8) * 8;	// "original" scrolling stripe}
	SDL_Rect stripe_rect = { ship_rect.x + pi->stripe_offset_.x, ship_rect.y + pi->stripe_offset_.y, pi->ship_texture_.getScaledRect().w, pi->stripe_texture_.getScaledRect().h };

	SDL_Rect taillight_rect = {};
	if ((timer->getTotalFrames() - 1) % 20 < 10)
	{
		if (orient.direction == HOrient::right)
			taillight_rect = { ship_rect.x, ship_rect.y, pi->taillight_rect_.w, pi->taillight_rect_.h };
		else
			taillight_rect = { ship_rect.x + ship_rect.w - pi->taillight_rect_.w, ship_rect.y, pi->taillight_rect_.w, pi->taillight_rect_.h };
	}

	SDL_Rect burner_rect = {};
	MappedTexture* burner_texture = nullptr;
	if (thrust.current.x != 0.0)
	{
		long burner_index = std::min(std::lround(4 * std::abs(thrust.current.x) / thrust.max.x), 3L);
		burner_texture = &pi->burner_textures_.at(burner_index);
		if (!burner_texture)
		{
			throw std::runtime_error{ "Unable to locate ship burner texture for index " + std::to_string(burner_index) + " thrust.current.x=" + std::to_string(thrust.current.x) + " thrust.max.x=" + std::to_string(thrust.max.x) };
		}

		if (thrust.current.x != 0.0 && orient.direction == HOrient::right)
		{
			burner_rect = { ship_rect.x + pi->burner_offset_.x, ship_rect.y + pi->burner_offset_.y, burner_texture->getScaledRect().w, burner_texture->getScaledRect().h };
		}
		else if (thrust.current.x != 0.0 && orient.direction == HOrient::left)
		{
			burner_rect = { ship_rect.x + ship_rect.w, ship_rect.y + pi->burner_offset_.y, burner_texture->getScaledRect().w, burner_texture->getScaledRect().h };
		}
	}

	SDL_Texture* wheels_texture = nullptr;
	SDL_Rect wheels_rect = {};
	float altitude = playfield->getAltitude(state.current.pos.y, phys.box.h);
	if (altitude == 0.0)
	{
		wheels_rect = { ship_rect.x + pi->wheels_offset_.x, ship_rect.y + pi->wheels_offset_.y, pi->wheels_texture_.getScaledRect().w, pi->wheels_texture_.getScaledRect().h };
		wheels_texture = pi->wheels_texture_.texture;
	}

	if (collide->is_collided)
		SDL_SetTextureColorMod(pi->ship_texture_.texture, 0, 0, 0);
	else
		SDL_SetTextureColorMod(pi->ship_texture_.texture, 255, 255, 255);

	SDL_RenderCopyEx(sdl_renderer, pi->ship_texture_.texture, &pi->ship_texture_.rect, &ship_rect, 0, NULL, flip);
	SDL_SetTextureColorMod(pi->ship_texture_.texture, 255, 255, 255); // shared texture, so reset to defaults

	SDL_RenderCopyEx(sdl_renderer, pi->stripe_texture_.texture, &stripe_texture_rect, &stripe_rect, 0, NULL, flip);
	if (renderutil::hasDimensions(taillight_rect))
	{
		SDL_SetRenderDrawColor(sdl_renderer, pi->taillight_color_.r, pi->taillight_color_.g, pi->taillight_color_.b, pi->taillight_color_.a);
		SDL_RenderFillRect(sdl_renderer, &taillight_rect);
	}
	if (renderutil::hasDimensions(burner_rect))
	{
		SDL_RenderCopyEx(sdl_renderer, burner_texture->texture, &burner_texture->rect, &burner_rect, 0, NULL, flip);
	}
	if (renderutil::hasDimensions(wheels_rect))
	{
		SDL_RenderCopyEx(sdl_renderer, wheels_texture, &pi->wheels_texture_.rect, &wheels_rect, 0, NULL, flip);
	}
}