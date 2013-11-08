#include "Renderer.h"
#include "Entity.h"
#include "Debug.h"
#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include "glew.h"

//#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>

const SDL_Color& CoCoPalette::getColor(const CoCoPaletteEnum colorEnum) const
{
	if (coco_palette.count(colorEnum) > 0)
		return coco_palette.at(colorEnum);
	else
		return coco_palette.at(black);
}

Camera::Camera(const SDL_Rect& view_rect, const SDL_Rect& focus_rect)
{
	this->view_rect = view_rect;
	this->focus_rect = focus_rect;
	this->focus_point = { 0, 0 };
}

Renderer::Renderer(int screen_width, int screen_height, float scaling)
{
	this->window.w = screen_width;
	this->window.h = screen_height;
	this->scaling = scaling;
	init();
}

void Renderer::init()
{
	sdlWindow = SDL_CreateWindow("Starblaze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.w, window.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
	toggleFullscreen(is_fullscreen);
	debug({ SDL_GetError() });
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	debug({ SDL_GetError() });
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_RenderSetLogicalSize(sdlRenderer, 1024, 768);
	//SDL_RenderSetLogicalSize(sdlRenderer, 256, 192);

	GLenum glew_init = glewInit();
	if (GLEW_OK != glew_init)
	{
		// GLEW failed!
		std::string glew_error = (char*)glewGetErrorString(glew_init);
		debug({ "GLEW initialization error: ", glew_error });
		exit(1);
	}
}

void Renderer::toggleFullscreen(bool state)
{
	is_fullscreen = state;
	if (state)
		SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(sdlWindow, 0);
}

void Renderer::toggleGrid(bool state)
{
	is_grid_visible = state;
}

SDL_Texture* Renderer::loadTextureFromFile(std::string imagePath, SDL_Rect* texture_rect)
{
	SDL_Surface* sdlSurface = IMG_Load(imagePath.c_str());
	debug({ SDL_GetError() });
	SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlSurface);
	SDL_FreeSurface(sdlSurface);
	SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
	if (texture_rect != nullptr)
	{
		texture_rect->x = 0;
		texture_rect->y = 0;
		texture_rect->w = sdlSurface->w;
		texture_rect->h = sdlSurface->h;
	};
	return sdlTexture;
}

void Renderer::render(Camera* camera)
{
	SDL_SetRenderDrawColor(sdlRenderer, 0, 31, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdlRenderer);

	for (auto& sprite : sprite_register._sprites)
	{
		sprite->render(this->sdlRenderer, *camera);
	}

	if (is_grid_visible)
		renderGrid();

	SDL_RenderPresent(sdlRenderer);
}

void Renderer::renderGrid()
{
	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	for (int x = 0; x <= this->window.w; x += 8)
	{
		int y1 = 8;
		int line_height = 0;
		if ((x % 64) < 32)
			line_height = 32 - (x % 32);
		else if ((x % 64) == 32)
			line_height = 16;
		else
			line_height = x % 32;
		SDL_RenderDrawLine(sdlRenderer, x, y1, x, y1 + line_height);
	}

	glLineWidth(2.0f);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x0101);
	for (int x = 0; x < this->window.w; x += 256)
	{
		SDL_RenderDrawLine(sdlRenderer, x, 0, x, window.h);
	}
	SDL_RenderDrawLine(sdlRenderer, window.w - 1, 0, window.w - 1, window.h);

	for (int y = 0; y <= this->window.h; y += 256)
	{
		SDL_RenderDrawLine(sdlRenderer, 0, y, window.w - 1, y);
	}
	SDL_RenderDrawLine(sdlRenderer, 0, window.h - 1, window.w, window.h - 1);
	glDisable(GL_LINE_STIPPLE);
}

SpriteRegister::SpriteRegister()
{

}

SpriteRegister::~SpriteRegister()
{

}

void SpriteRegister::registerSprite(Sprite* sprite)
{
	this->_sprites.push_back(sprite);
}

Sprite::Sprite()
{

}

Sprite::~Sprite()
{

}

ShipSprite::ShipSprite(Renderer* renderer, Ship* ship) : Sprite()
{
	this->_scaling = renderer->scaling;

	this->_ship = std::shared_ptr<Ship>(ship);
	this->_stripe_texture_rect = { 0, 0, 32, 1 };
	this->_burner_texture_rect = { 0, 0, 8, 5 };
	this->_burner_rev_texture_rect = { 0, 0, 4, 4 };

	this->_ship_texture = renderer->loadTextureFromFile("img\\ship.tga", &this->_ship_texture_rect);
	this->_stripe_texture = renderer->loadTextureFromFile("img\\shipstripe.tga", nullptr);
	this->_taillight_texture = renderer->loadTextureFromFile("img\\shiptaillight.tga", &this->_taillight_texture_rect);
	this->_burner_texture = renderer->loadTextureFromFile("img\\shipburner.tga", nullptr);
	this->_burner_rev_texture = renderer->loadTextureFromFile("img\\shipburner_rev.tga", nullptr);
	this->_wheels_texture = renderer->loadTextureFromFile("img\\shipwheels.tga", &this->_wheels_texture_rect);

	this->_stripe_offset = { 0, 6 * _scaling };
	this->_burner_offset = { -_burner_texture_rect.w  * _scaling, (_ship_texture_rect.h - 5)  * _scaling };
	this->_burner_rev_offset = { 17 * _scaling, 4 * _scaling };
	this->_wheels_offset = { 0, _ship_texture_rect.h * _scaling };

	this->smooth_animation = false;
}

ShipSprite::~ShipSprite()
{

}

void ShipSprite::render(SDL_Renderer* sdlRenderer, const Camera& camera)
{
	SDL_RendererFlip flip = _ship->direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RendererFlip flipReverse = flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	
	SDL_Rect ship_rect = { camera.focus_point.x - camera.view_rect.x, camera.focus_point.y - camera.view_rect.y, _ship_texture_rect.w  * _scaling, _ship_texture_rect.h  * _scaling };

	if (smooth_animation)
		_stripe_texture_rect.y = (timer->getTotalFrames() - 1) % 32;				// smooth scrolling stripe
	else
		_stripe_texture_rect.y = (((timer->getTotalFrames() - 1) % 32) / 8) * 8;	// "original" scrolling stripe}
	SDL_Rect stripe_rect = { ship_rect.x + _stripe_offset.x, ship_rect.y + _stripe_offset.y, _stripe_texture_rect.w  * _scaling, _stripe_texture_rect.h  * _scaling };

	SDL_Rect taillight_rect = {};
	SDL_Texture* taillight_texture = nullptr;
	if ((timer->getTotalFrames() - 1) % 20 < 10)
	{
		taillight_rect = { ship_rect.x, ship_rect.y, _taillight_texture_rect.w  * _scaling, _taillight_texture_rect.h  * _scaling };
		taillight_texture = this->_taillight_texture;
	}

	//debug({ "Ship thrust/vel X:", std::to_string(_ship->current_state.thrust.x), "/", std::to_string(_ship->current_state.vel.x), " accel Y:", std::to_string(_ship->current_state.acc.y), " direction:", std::to_string((int)_ship->direction) });
	SDL_Rect burner_rect = {};
	SDL_Texture* burner_texture = nullptr; // may be either forward or reverse
	SDL_Rect burner_texture_rect = {};
	if (_ship->current_state.thrust.x != 0.0)
	{
		_burner_texture_rect.x = std::min((int)((_ship->max_thrust / abs(_ship->current_state.thrust.x))) - 1, 3) * 8;
		_burner_rev_texture_rect.x = std::min((int)(abs(_ship->current_state.thrust.x) / 8.0), 1) * 4;
		if ((_ship->current_state.thrust.x > 0.0 && _ship->direction == ShipDirection::right) || (_ship->current_state.thrust.x < 0.0 && _ship->direction == ShipDirection::left))
		{
			burner_texture = _burner_texture;
			burner_texture_rect = _burner_texture_rect;
		}
		else
		{
			burner_texture = _burner_rev_texture;
			burner_texture_rect = _burner_rev_texture_rect;
		}

		if (_ship->current_state.thrust.x > 0.0 && _ship->direction == ShipDirection::right)
		{
			burner_rect = { ship_rect.x + _burner_offset.x, ship_rect.y + _burner_offset.y, _burner_texture_rect.w  * _scaling, _burner_texture_rect.h  * _scaling };
		}
		else if (_ship->current_state.thrust.x < 0.0 && _ship->direction == ShipDirection::left)
		{
			burner_rect = { ship_rect.x + ship_rect.w, ship_rect.y + _burner_offset.y, _burner_texture_rect.w  * _scaling, _burner_texture_rect.h  * _scaling };
		}
		else if (_ship->current_state.thrust.x < 0.0 && _ship->direction == ShipDirection::right)
		{
			burner_rect = { ship_rect.x + _burner_rev_offset.x, ship_rect.y + _burner_rev_offset.y, _burner_rev_texture_rect.w  * _scaling, _burner_rev_texture_rect.h  * _scaling };
		}
		else if (_ship->current_state.thrust.x > 0.0 && _ship->direction == ShipDirection::left)
		{
			burner_rect = { ship_rect.x + ship_rect.w - (_burner_rev_texture_rect.w  * _scaling) - _burner_rev_offset.x, ship_rect.y + _burner_rev_offset.y, _burner_rev_texture_rect.w  * _scaling, _burner_rev_texture_rect.h  * _scaling };
		}
	}

	SDL_Texture* wheels_texture = nullptr;
	SDL_Rect wheels_rect = {};
	if (_ship->altitude == 0.0 && std::abs(_ship->current_state.vel.x) < _ship->takeoff_speed)
	{
		wheels_rect = { ship_rect.x + _wheels_offset.x, ship_rect.y + _wheels_offset.y, _wheels_texture_rect.w * _scaling, _wheels_texture_rect.h * _scaling };
		wheels_texture = this->_wheels_texture;
	}

	SDL_RenderCopyEx(sdlRenderer, _ship_texture, &_ship_texture_rect, &ship_rect, 0, NULL, flip);
	SDL_RenderCopyEx(sdlRenderer, _stripe_texture, &_stripe_texture_rect, &stripe_rect, 0, NULL, flipReverse);
	if (taillight_texture)
		SDL_RenderCopyEx(sdlRenderer, taillight_texture, &_taillight_texture_rect, &taillight_rect, 0, NULL, flip);
	if (burner_texture)
		SDL_RenderCopyEx(sdlRenderer, burner_texture, &burner_texture_rect, &burner_rect, 0, NULL, flip);
	if (wheels_texture)
		SDL_RenderCopyEx(sdlRenderer, wheels_texture, &_wheels_texture_rect, &wheels_rect, 0, NULL, flip);
}

BGSprite::BGSprite(Renderer* renderer, World* world) : Sprite()
{
	_scaling = renderer->scaling;
	_world = std::shared_ptr<World>(world);
	_sky_rect = { 0, 0, renderer->window.w, 96 * _scaling };
	_sky_color = renderer->coco_palette.getColor(blue);
	_ground_rect = { 0, _sky_rect.h, renderer->window.w, 48 * _scaling };
	_ground_color = renderer->coco_palette.getColor(green);
	_hills_texture = renderer->loadTextureFromFile("img\\hills_highlight.tga", nullptr);
	_hills_texture_rect = { 0, 0, 16, 4 };
	_hills_rect = { 0, _ground_rect.y, renderer->window.w, 28 * _scaling };
	_bg_rect = { 0, 0, renderer->window.w, renderer->window.h };
	_bg_color = renderer->coco_palette.getColor(green);
}

BGSprite::~BGSprite()
{

}

void BGSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera)
{
	const std::array<int, 4> y_channel_coords = { { 0 - _hills_texture_rect.h * _scaling, 9 * _scaling, 17 * _scaling, 25 * _scaling } };
	const std::array<double, 4> y_channel_speeds = { { 0.1, 0.25, 0.5, 0.75 } };
	SDL_SetRenderDrawColor(sdl_renderer, _bg_color.r, _bg_color.g, _bg_color.b, _bg_color.a);
	SDL_RenderFillRect(sdl_renderer, &_bg_rect);
	SDL_SetRenderDrawColor(sdl_renderer, _sky_color.r, _sky_color.g, _sky_color.b, _sky_color.a);
	SDL_RenderFillRect(sdl_renderer, &_sky_rect);
	SDL_SetRenderDrawColor(sdl_renderer, _ground_color.r, _ground_color.g, _ground_color.b, _ground_color.a);
	SDL_RenderFillRect(sdl_renderer, &_ground_rect);
	for (auto hill : _world->hills)
	{
		_hills_texture_rect.y = _hills_texture_rect.h * hill.type;
		SDL_Rect hill_rect = { hill.x_channel * 8 * _scaling, _hills_rect.y + y_channel_coords[hill.y_channel], _hills_texture_rect.w * _scaling, _hills_texture_rect.h * _scaling };
		hill_rect.x -= camera.view_rect.x * y_channel_speeds[hill.y_channel];
		//debug({ "Hill rect X:", std::to_string(hill_rect.x), " Y:", std::to_string(hill_rect.y) });
		SDL_RenderCopy(sdl_renderer, _hills_texture, &_hills_texture_rect, &hill_rect);
	}
}

RadarSprite::RadarSprite(Renderer* renderer)
{
	_scaling = renderer->scaling;
	_radar_rect = { 64 * _scaling, 156 * _scaling, 128 * _scaling, 36 * _scaling };
	_view_points = { {
		{ 56 * _scaling, 0 * _scaling },
		{ 70 * _scaling, 0 * _scaling },
		{ 56 * _scaling, 34 * _scaling },
		{ 70 * _scaling, 34 * _scaling }
	} };
	_radar_color = renderer->coco_palette.getColor(red);
	_point_color = renderer->coco_palette.getColor(yellow);
}

void RadarSprite::render(SDL_Renderer* sdl_renderer, const Camera& camera)
{
	SDL_SetRenderDrawColor(sdl_renderer, _radar_color.r, _radar_color.g, _radar_color.b, _radar_color.a);
	SDL_RenderFillRect(sdl_renderer, &_radar_rect);

	SDL_SetRenderDrawColor(sdl_renderer, _point_color.r, _point_color.g, _point_color.b, _radar_color.a);
	for (auto& vp : _view_points)
	{
		SDL_Rect point_rect = { _radar_rect.x + vp.x, _radar_rect.y + vp.y, 2 * _scaling, 2 * _scaling };
		SDL_RenderFillRect(sdl_renderer, &point_rect);
	}
}