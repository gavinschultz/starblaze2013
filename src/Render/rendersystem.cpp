#include "stdafx.h"
#include <memory>
#include <sstream>
#include <vector>
#include <SDL_ttf.h>
#include <glew.h>
#include "sdlutil.h"
#include "rendersystem.h"
#include "program.h"
#include "cocopalette.h"
#include "timer.h"
#include "spriteloader.h"
#include "component.h"
#include "session.h"
#include "renderdefs.h"
#include "renderutil.h"
#include "phys.h"
#include "prefs.h"
#include "bmfontloader.h"
#include "playfield.h"
#include "motionhistory.h"

class RenderSystem::impl
{
public:
	impl();
	~impl();
	Window window;
	SDL_Window* sdl_window{ nullptr };
	SDL_Renderer* sdl_renderer{ nullptr };

	std::unique_ptr<SpriteLoader> sprite_loader;

	bool is_fullscreen{ false };

	TTF_Font* debug_font{ nullptr };

	void renderDebug(const std::vector<debug::DebugItem>& items) const;

	std::unique_ptr<BackgroundRender> background_render;
	std::unique_ptr<FPSRender> fps_render;
	std::unique_ptr<DebugRender> debug_render;
	std::unique_ptr<MotionHistoryRender> motionhistory_render;
	std::unique_ptr<GridRender> grid_render;
	std::unique_ptr<ZeroLineRender> zeroline_render;
	std::unique_ptr<ShipRender> ship_render;
	std::unique_ptr<HUDRender> hud_render;
	std::unique_ptr<RadarRender> radar_render;
	std::unique_ptr<TextRender> text_render;
	std::unique_ptr<CollisionBoxRender> collisionbox_render;
	std::unique_ptr<StationRender> station_render;
};

RenderSystem::impl::impl() = default;
RenderSystem::impl::~impl() = default;

RenderSystem::RenderSystem(Window window) : pi{ new impl{} }
{
	pi->window = window;
	init();
}
RenderSystem::~RenderSystem() {}

void RenderSystem::init()
{
	debug::console({ "Initializing RenderSystem..." });
	
	if (pi->sdl_renderer)
	{
		SDL_DestroyRenderer(pi->sdl_renderer);
	}

	if (pi->sdl_window)
	{
		SDL_DestroyWindow(pi->sdl_window);
	}

	pi->sprite_loader = std::make_unique<SpriteLoader>();

	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	if (!(pi->sdl_window = SDL_CreateWindow("unnamed", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pi->window.w, pi->window.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL)))
	{
		program::exit(RetCode::sdl_error, { SDL_GetError() });
	}

	setFullscreen(false);
	Uint32 flags = SDL_RENDERER_ACCELERATED;
	if (prefs::vsync)
		flags |= SDL_RENDERER_PRESENTVSYNC;
	if (!(pi->sdl_renderer = SDL_CreateRenderer(pi->sdl_window, -1, flags)))
	{
		program::exit(RetCode::sdl_error, { SDL_GetError() });
	}

	pi->sprite_loader->load(pi->sdl_renderer, "resources\\spritesheet.json");

	int logical_w = 1366;
	int logical_h = 768;
	if (SDL_RenderSetLogicalSize(pi->sdl_renderer, logical_w, logical_h))
	{
		program::exit(RetCode::sdl_error, { "Error setting logical size (", std::to_string(logical_w), ",", std::to_string(logical_h), ")", SDL_GetError() });
	}

	GLenum glew_init = glewInit();
	if (glew_init != GLEW_OK)
	{
		std::string glew_error = (char*)glewGetErrorString(glew_init);
		program::exit(RetCode::runtime_error, { "GLEW initialization error: ", glew_error });
	}

	if (TTF_Init() == -1)
	{
		program::exit(RetCode::sdl_error, { "Error initializing SDL_TTF: %s\n", TTF_GetError() });
	}

	if (!(pi->debug_font = TTF_OpenFont("resources\\ostrich-black.ttf", 48)))
	{
		program::exit(RetCode::sdl_error, { "Error opening debug font: %s\n", TTF_GetError() });
	}

	palette = SDL_AllocPalette(11);
	SDL_SetPaletteColors(palette, CoCoPalette::colors.data(), 0, 11);

	bmfont::load(pi->sdl_renderer, "resources\\font-ostrich-black.fnt");

	debug::console({ "Renderer info\n--------------\n", getInfo() });

	pi->background_render = std::make_unique<BackgroundRender>(*this);
	pi->fps_render = std::make_unique<FPSRender>(pi->debug_font, pi->window);
	pi->debug_render = std::make_unique<DebugRender>(pi->debug_font);
	pi->motionhistory_render = std::make_unique<MotionHistoryRender>();
	pi->grid_render = std::make_unique<GridRender>();
	pi->zeroline_render = std::make_unique<ZeroLineRender>();
	pi->ship_render = std::make_unique<ShipRender>(*this);
	pi->hud_render = std::make_unique<HUDRender>(*this);
	pi->radar_render = std::make_unique<RadarRender>(*this);
	pi->text_render = std::make_unique<TextRender>(*this, pi->sprite_loader->getSprite("characters"));
	pi->collisionbox_render = std::make_unique<CollisionBoxRender>();
	pi->station_render = std::make_unique<StationRender>(*this);
}

void RenderSystem::draw(Camera& camera)
{
	SDL_RenderClear(pi->sdl_renderer);
	SDL_SetRenderDrawColor(pi->sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	auto playfield = db->getPlayField();
	if (playfield)
	{
		pi->background_render->render(pi->sdl_renderer, camera, *playfield);
	}

	for (auto id : db->getEntitiesOfType(E::estation))
	{
		auto station_st = (StationComponent*)db->getComponentOfTypeForEntity(id, C::cstation);
		auto station_state = (TemporalState2DComponent*)db->getComponentOfTypeForEntity(id, C::ctemporalstate);
		auto station_phys = (PhysicalComponent*)db->getComponentOfTypeForEntity(id, C::cphysical);
		pi->station_render->render(pi->sdl_renderer, camera, *station_st, *station_state, *station_phys);
	}

	//for (auto e : db->getEntitiesOfType(E::alien))
	//{
	//	alien_render.draw(pi->sdl_renderer, camera, (Alien*)e);
	//}

	auto player_id = db->getEntitiesOfType(E::eship)[0];
	auto player_body = (PoweredBodyComponent*)db->getComponentOfTypeForEntity(player_id, C::cpoweredbody);
	auto player_orient = (HorizontalOrientComponent*)db->getComponentOfTypeForEntity(player_id, C::chorient);
	auto player_info = (PlayerComponent*)db->getComponentOfTypeForEntity(player_id, C::cplayer);
	auto player_thrust = (ThrustComponent*)db->getComponentOfTypeForEntity(player_id, C::cthrust);
	auto player_state = (TemporalState2DComponent*)db->getComponentOfTypeForEntity(player_id, C::ctemporalstate);
	auto player_phys = (PhysicalComponent*)db->getComponentOfTypeForEntity(player_id, C::cphysical);
	if (db->hasEntity(E::eship))
	{
		pi->ship_render->render(pi->sdl_renderer, camera, *player_state, *player_orient, *player_thrust, *player_phys);
	}

	//for (auto e : db->getEntitiesOfType(E::edebris))
	//{
	//	debris_render.draw(pi->sdl_renderer, camera, (Debris*)e);
	//}

	//for (auto e : db->getEntitiesOfType(E::ebullet))
	//{
	//	bullet_render.draw(pi->sdl_renderer, camera, (Bullet*)e);
	//}

	
	pi->hud_render->render(pi->sdl_renderer, *player_info, session::score, session::lives, *pi->text_render);

	pi->radar_render->renderBox(pi->sdl_renderer);
	for (auto c : db->getComponentsOfType(C::cradartrackable))
	{
		pi->radar_render->render(pi->sdl_renderer, camera, *(RadarTrackableComponent*)c);
	}

	for (auto c : db->getComponentsOfType(C::ctextplate))
	{
		auto textplate_component = (TextPlateComponent*)c;
		pi->text_render->renderPlate(pi->sdl_renderer, *textplate_component->textplate.get(), palette->colors[CoCoPaletteEnum::yellow]);
	}

	if (prefs::show_fps)
	{
		pi->fps_render->render(pi->sdl_renderer, timer->getFrameRate());
	}

	if (prefs::show_debug)
	{
		pi->debug_render->render(pi->sdl_renderer, debug::getItems());
	}

	if (prefs::show_grid)
	{
		Rect playfield_rect;
		if (playfield)
			playfield_rect = playfield->boundaries;
		else
			playfield_rect = { 0, 0, 0, 0 };
		pi->grid_render->render(pi->sdl_renderer, pi->window, playfield_rect);
		pi->zeroline_render->render(pi->sdl_renderer, pi->window, camera);
	}

	if (prefs::show_collision_boxes)
	{
		for (auto c : db->getComponentsOfType(C::ccollision))
		{
			pi->collisionbox_render->render(pi->sdl_renderer, camera, *(CollisionComponent*)c);
		}
	}

	if (prefs::show_motionhistory)
	{
		pi->motionhistory_render->render(pi->sdl_renderer, pi->window, motionhistory::get(), motionhistory::getThresholds(), motionhistory::getCurrentIndex());
	}

	SDL_RenderPresent(pi->sdl_renderer);
}

bool RenderSystem::isFullscreen() const
{
	return pi->is_fullscreen;
}

void RenderSystem::setFullscreen(bool state)
{
	pi->is_fullscreen = state;
	int flag;
	if (state)
		flag = SDL_WindowFlags::SDL_WINDOW_FULLSCREEN; // SDL_WindowFlags::SDL_WINDOW_FULLSCREEN_DESKTOP;
	else
		flag = 0;

	if (SDL_SetWindowFullscreen(pi->sdl_window, flag))
	{
		debug::console({ "Error setting fullscreen to ", std::to_string(state), ": ", SDL_GetError() });
	}
}

const SpriteLoader& RenderSystem::getSpriteLoader() const
{
	return *(pi->sprite_loader);
}

std::string RenderSystem::getInfo() const
{
	std::stringstream ss;
	SDL_RendererInfo renderer_info;
	if (SDL_GetRendererInfo(pi->sdl_renderer, &renderer_info))
	{
		debug::console({ "Error getting SDL renderer info: ", SDL_GetError() });
		return "Error getting SDL renderer info";
	}

	int logical_w, logical_h;
	SDL_RenderGetLogicalSize(pi->sdl_renderer, &logical_w, &logical_h);

	ss << "Renderer name:        " << renderer_info.name << "\n";
	ss << "Logical window size:  " << "(" << logical_w << "," << logical_h << ")\n";
	ss << "Actual window size:   " << "(" << pi->window.w << "," << pi->window.h << ")\n";
	ss << "OpenGL version:       " << glGetString(GL_VERSION) << "\n";
	ss << "Is fullscreen:        " << (isFullscreen() ? "yes" : "no") << "\n";
	ss << "Max texture height:   " << renderer_info.max_texture_height << "\n";
	ss << "Max texture width:    " << renderer_info.max_texture_width << "\n";
	ss << "Hardware accelerated: " << ((renderer_info.flags & SDL_RendererFlags::SDL_RENDERER_ACCELERATED) ? "yes" : "no") << "\n";
	ss << "Software fallback:    " << ((renderer_info.flags & SDL_RendererFlags::SDL_RENDERER_SOFTWARE) ? "yes" : "no") << "\n";
	ss << "VSYNC enabled:        " << ((renderer_info.flags & SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC) ? "yes" : "no") << "\n";
	ss << "Render to texture:    " << ((renderer_info.flags & SDL_RendererFlags::SDL_RENDERER_TARGETTEXTURE) ? "yes" : "no") << "\n";

	for (unsigned int i = 0; i < renderer_info.num_texture_formats; i++)
	{
		Uint32 texture_format = renderer_info.texture_formats[i];
		ss << "Texture format " << i << ":     " << SDL_GetPixelFormatName(texture_format) << " " << (SDL_ISPIXELFORMAT_FOURCC(texture_format) ? "*" : "") << "\n";
		ss << "  Pixel type:         " << sdlutil::getPixelType(texture_format) << "\n";
		ss << "  Pixel order:        " << sdlutil::getPixelOrder(texture_format) << "\n";
		ss << "  Pixel layout:       " << sdlutil::getPixelLayout(texture_format) << "\n";
		ss << "  Bits per pixel:     " << SDL_BITSPERPIXEL(texture_format) << "\n";
		ss << "  Bytes per pixel:    " << SDL_BYTESPERPIXEL(texture_format) << "\n";
		ss << "  Has palette:        " << (SDL_ISPIXELFORMAT_INDEXED(texture_format) ? "yes" : "no") << "\n";
		ss << "  Has alpha:          " << (SDL_ISPIXELFORMAT_ALPHA(texture_format) ? "yes" : "no") << "\n";
	}

	return ss.str();
}

const Window& RenderSystem::getWindow() const
{
	return pi->window;
}