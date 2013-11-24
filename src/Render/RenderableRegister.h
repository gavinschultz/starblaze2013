//#pragma once
//#include <SDL.h>
//#include "Camera.h"
//
//class SDLRenderable
//{
//protected:
//	SDLRenderable(SDL_Renderer* sdl_renderer) : sdl_renderer{ sdl_renderer } {}
//	SDL_Renderer* sdl_renderer;
//};
//
//class SDLBackgroundRenderable : public SDLRenderable
//{
//public:
//	SDLBackgroundRenderable(SDL_Renderer* sdl_renderer);
//	void render(const Camera& camera) const;
//};
//
//SDLBackgroundRenderable::SDLBackgroundRenderable(SDL_Renderer* sdl_renderer) : SDLRenderable(sdl_renderer)
//{
//
//}
//
//void SDLBackgroundRenderable::render(const Camera& camera) const
//{
//
//}
//
//class RenderableRegister
//{
//public:
//	const SDLBackgroundRenderable& getBackground();
//};



//
//render_register.getBackground().render(camera);
//
//sprite_register.getRadarSprite().render(sdl_renderer, camera, entity_register, world);
//
//for (auto& enemy = entity_register.getEnemies())
//{
//	auto& enemySprite = sprite_register.getEnemySprite(enemy);
//	enemySprite.render(sdl_renderer, camera, enemy);
//}
//
//sprite_register.getPlayerSprite().render(sdl_renderer, camera, entity_register.getPlayerShip());
//
//sprite_register.getHUDSprite().render(sdl_renderer, camera, entity_register.getPlayerShip(), game->score);
//
//auto& station = entity_register.getSTation());
//if (station)
//{
//	sprite_register.getStationSprite(station).render(sdl_renderer, camera, entity_register.getStation());
//}
//
//if (game->show_map)
//{
//	sprite_register.getMap().render(sdl_renderer, game->map);
//}
//
//if (game->show_titles)
//{
//	_text_renderer->RenderPlate(sdl_renderer, sprite_register.getTitlePlate());
//}