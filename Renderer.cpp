#include "Renderer.h"
#include "Entity.h"
#include "Debug.h"
#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include "glew.h"
#include "Util.h"

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

Renderer::Renderer(uint32_t screen_width, uint32_t screen_height, uint32_t scaling, double world_width) : scaling(scaling), width((uint32_t)(world_width*scaling))
{
	this->window.w = screen_width;
	this->window.h = screen_height;
	init();
}

Renderer::~Renderer()
{
	TTF_CloseFont(_font);
	TTF_Quit();
}

void Renderer::init()
{
	sdlWindow = SDL_CreateWindow("Starblaze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.w, window.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
	toggleFullscreen(is_fullscreen);
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_RenderSetLogicalSize(sdlRenderer, 1024, 768);

	GLenum glew_init = glewInit();
	if (GLEW_OK != glew_init)
	{
		// GLEW failed!
		std::string glew_error = (char*)glewGetErrorString(glew_init);
		console_debug({ "GLEW initialization error: ", glew_error });
		exit(1);
	}

	if (TTF_Init() == -1) {
		console_debug({ "TTF_Init: %s\n", TTF_GetError() });
		exit(2);
	}
	_font = TTF_OpenFont("DroidSans.ttf", 16);
	if (!_font) {
		console_debug({ "TTF_OpenFont: %s\n", TTF_GetError() });
		exit(3);
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

void Renderer::toggleMotionHistory(bool state)
{
	is_motionhistory_visible = state;
}

// only works for absolute coordinates
bool Renderer::isRightOf(int32_t x, int32_t y)
{
	if ((x - y) >= 0)
		return ((x - y) % width) < width / 2;
	else
		return ((y - x) % width) > width / 2;
}

bool Renderer::isLeftOf(int32_t x, int32_t y) { 
	return !isRightOf(x, y); 
}

SDL_Texture* Renderer::loadTextureFromFile(std::string imagePath, SDL_Rect* texture_rect)
{
	SDL_Surface* sdlSurface = IMG_Load(imagePath.c_str());
	if (!sdlSurface)
	{
		console_debug({ SDL_GetError() });
		return nullptr;
	}
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
	{
		renderGrid();
		renderFPS((int)timer->getFrameRate());
		renderDebug(*debug.get());
		renderZeroLine(*camera);
	}

	if (is_motionhistory_visible)
		renderMotionHistory(*debug.get());

	SDL_RenderPresent(sdlRenderer);
}

void Renderer::renderZeroLine(const Camera& camera)
{
	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	int view_rect_abs_x;
	if (camera.view_rect.x < 0)
		view_rect_abs_x = camera.view_rect.x + this->width;
	else
		view_rect_abs_x = camera.view_rect.x;
	int zero_point_x = this->width - view_rect_abs_x;
	SDL_RenderDrawLine(sdlRenderer, zero_point_x, 0, zero_point_x, window.h);
}

void Renderer::renderGrid()
{
	glLineWidth(2.0f);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	for (uint32_t x = 0; x <= this->window.w; x += 8)
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
	for (uint32_t x = 0; x < this->window.w; x += 256)
	{
		SDL_RenderDrawLine(sdlRenderer, x, 0, x, window.h);
	}
	SDL_RenderDrawLine(sdlRenderer, window.w - 1, 0, window.w - 1, window.h);

	for (uint32_t y = 0; y <= this->window.h; y += 256)
	{
		SDL_RenderDrawLine(sdlRenderer, 0, y, window.w - 1, y);
	}
	SDL_RenderDrawLine(sdlRenderer, 0, window.h - 1, window.w, window.h - 1);

	SDL_SetRenderDrawColor(sdlRenderer, 64, 64, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(sdlRenderer, 48 * scaling, 0, 48 * scaling, window.h);
	SDL_RenderDrawLine(sdlRenderer, 208 * scaling, 0, 208 * scaling, window.h);
	glDisable(GL_LINE_STIPPLE);
}

void Renderer::renderText(const std::string text, uint32_t x, uint32_t y)
{
	SDL_Color text_color = { 0, 0, 0 };
	SDL_Surface* text_surface = TTF_RenderText_Blended(_font, text.c_str(), text_color);
	SDL_Rect text_texture_rect = { 0, 0, text_surface->w, text_surface->h };
	SDL_Rect text_rect = { x, y, text_surface->w, text_surface->h };
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(sdlRenderer, text_surface);
	SDL_FreeSurface(text_surface);
	SDL_RenderCopy(sdlRenderer, text_texture, &text_texture_rect, &text_rect);
	SDL_DestroyTexture(text_texture);
}

void Renderer::renderFPS(int fps)
{
	static int cached_fps;
	if (timer->getTotalFrames() % 10 == 0)
		cached_fps = fps;
	this->renderText("FPS:" + std::to_string(fps), 50, 50);
}

void Renderer::renderDebug(const Debug& debug)
{
	//TODO: probably horribly inefficient

	const int initial_y = 100;
	int y = initial_y;
	int text_w, text_h;
	int max_text_w = 0;
	int font_height = TTF_FontHeight(_font);
	for (auto& i : debug.items)
	{
		std::string text = i->label + ": ";
		this->renderText(text, 50, y);
		TTF_SizeText(_font, text.c_str(), &text_w, &text_h);
		max_text_w = std::max(max_text_w, text_w);
		y += font_height + 4;
	}

	y = initial_y;
	for (auto& i : debug.items)
	{
		this->renderText(i->value, max_text_w + 50, y);
		y += font_height + 4;
	}
}

void Renderer::renderMotionHistory(const Debug& debug)
{
	const int initial_y = (window.h / 2);
	const int threshold_line_offset = 75;
	const int axis_line_offset = 100;

	glLineWidth(1.0f);

	// background
	SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 176);
	auto bgrect = SDL_Rect{ 0, initial_y - axis_line_offset - threshold_line_offset - 50, window.w, (axis_line_offset + threshold_line_offset + 50) * 2 };
	SDL_RenderFillRect(sdlRenderer, &bgrect);

	// base lines
	SDL_SetRenderDrawColor(sdlRenderer, 128, 128, 128, 255);
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y - axis_line_offset, window.w, initial_y - axis_line_offset);
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y + axis_line_offset, window.w, initial_y + axis_line_offset);

	// threshold lines
	SDL_SetRenderDrawColor(sdlRenderer, 64, 64, 64, 255);
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y - axis_line_offset - threshold_line_offset, window.w, initial_y - axis_line_offset - threshold_line_offset); // X motion - upper threshold
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y + axis_line_offset - threshold_line_offset, window.w, initial_y + axis_line_offset - threshold_line_offset); // X motion - lower threshold
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y - axis_line_offset + threshold_line_offset, window.w, initial_y - axis_line_offset + threshold_line_offset); // Y motion - upper threshold
	SDL_RenderDrawLine(sdlRenderer, 0, initial_y + axis_line_offset + threshold_line_offset, window.w, initial_y + axis_line_offset + threshold_line_offset); // Y motion - lower threshold

	// points
	Vector2Di render_point_y{ 0, 0 };
	Vector2Di prev_render_point_y;
	for (unsigned int i = 0; i < debug.motion_history.size(); i++)
	{
		Vector2D p = debug.motion_history[i];
		glPointSize(1.0f);

		int scaled_xmotion = std::lround(((double)threshold_line_offset / debug.motion_max_thresholds.x) * p.x); // X motion
		int scaled_ymotion = std::lround(((double)threshold_line_offset / debug.motion_max_thresholds.y) * p.y); // Y motion

		if (i > 0)
		{
			prev_render_point_y = render_point_y;
			render_point_y = { initial_y - axis_line_offset - scaled_xmotion, initial_y + axis_line_offset + scaled_ymotion };
			Vector2D prev_p = debug.motion_history[i - 1];

			if (std::abs(scaled_xmotion) > threshold_line_offset)
			{
				SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(sdlRenderer, i, initial_y - axis_line_offset - (threshold_line_offset*util::getsign(scaled_xmotion)));
			}
			else
			{
				int r, g, b;
				int frame_age = (int)debug.motion_history_counter - (int)i;
				if (frame_age < 0)
					frame_age += 1024;	// TODO: make variable
				b = 255 - std::min(frame_age * 2, 255);
				g = 155 + std::min(frame_age * 2, 100);
				r = 64 + std::min(frame_age, 191);
				//console_debug({ "frame_age: ", std::to_string(frame_age), "r, g, b: ", std::to_string(r), " , ", std::to_string(g), " , ", std::to_string(b) });
				SDL_SetRenderDrawColor(sdlRenderer, r, g, b, 255);
			}

			SDL_RenderDrawLine(sdlRenderer, i - 1, prev_render_point_y.x, i, render_point_y.x);

			SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 0, 255);
			SDL_RenderDrawLine(sdlRenderer, i - 1, prev_render_point_y.y, i, render_point_y.y);
			/*SDL_RenderDrawPoint(sdlRenderer, i, initial_y - axis_line_offset - scaled_xmotion);
			SDL_RenderDrawPoint(sdlRenderer, i, initial_y + axis_line_offset + scaled_ymotion);*/
		}
	}
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

	this->_stripe_offset = { 0, 6 * (int32_t)_scaling };
	this->_burner_offset = { -_burner_texture_rect.w  * (int32_t)_scaling, (_ship_texture_rect.h - 5)  * (int32_t)_scaling };
	this->_burner_rev_offset = { 17 * (int32_t)_scaling, 4 * (int32_t)_scaling };
	this->_wheels_offset = { 0, _ship_texture_rect.h * (int32_t)_scaling };

	this->smooth_animation = false;
}

ShipSprite::~ShipSprite()
{

}

void ShipSprite::render(SDL_Renderer* sdlRenderer, const Camera& camera)
{
	SDL_RendererFlip flip = _ship->direction == ShipDirection::left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RendererFlip flipReverse = flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	int32_t entity_x = render::getScreenXForEntityByCameraAndDistance(_ship->alpha_pos.x*(double)_scaling, _ship_texture_rect.w*_scaling, renderer->width, camera, 1.0);

	SDL_Rect ship_rect = { entity_x, std::lround(_ship->alpha_pos.y * _scaling) - camera.view_rect.y, _ship_texture_rect.w  * _scaling, _ship_texture_rect.h  * _scaling };
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

		if (_ship->current_state.thrust.x != 0.0 && _ship->direction == ShipDirection::right)
		{
			burner_rect = { ship_rect.x + _burner_offset.x, ship_rect.y + _burner_offset.y, _burner_texture_rect.w  * _scaling, _burner_texture_rect.h  * _scaling };
		}
		else if (_ship->current_state.thrust.x != 0.0 && _ship->direction == ShipDirection::left)
		{
			burner_rect = { ship_rect.x + ship_rect.w, ship_rect.y + _burner_offset.y, _burner_texture_rect.w  * _scaling, _burner_texture_rect.h  * _scaling };
		}

		/* reverse texture - disabled for now
		else if (_ship->current_state.thrust.x < 0.0 && _ship->direction == ShipDirection::right)
		{
			burner_rect = { ship_rect.x + _burner_rev_offset.x, ship_rect.y + _burner_rev_offset.y, _burner_rev_texture_rect.w  * _scaling, _burner_rev_texture_rect.h  * _scaling };
		}
		else if (_ship->current_state.thrust.x > 0.0 && _ship->direction == ShipDirection::left)
		{
			burner_rect = { ship_rect.x + ship_rect.w - (_burner_rev_texture_rect.w  * _scaling) - _burner_rev_offset.x, ship_rect.y + _burner_rev_offset.y, _burner_rev_texture_rect.w  * _scaling, _burner_rev_texture_rect.h  * _scaling };
		}
		*/
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
	/*if (burner_rev_texture)
		SDL_RenderCopyEx(sdlRenderer, burner_rev_texture, &burner_rev_texture_rect, &burner_rect, 0, NULL, flip);*/
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
	_hills_texture = renderer->loadTextureFromFile("img\\hills.tga", nullptr);
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
	SDL_SetRenderDrawColor(sdl_renderer, _bg_color.r, _bg_color.g, _bg_color.b, _bg_color.a);
	SDL_RenderFillRect(sdl_renderer, &_bg_rect);
	SDL_SetRenderDrawColor(sdl_renderer, _sky_color.r, _sky_color.g, _sky_color.b, _sky_color.a);
	SDL_RenderFillRect(sdl_renderer, &_sky_rect);
	SDL_SetRenderDrawColor(sdl_renderer, _ground_color.r, _ground_color.g, _ground_color.b, _ground_color.a);
	SDL_RenderFillRect(sdl_renderer, &_ground_rect);

	for (auto hill : _world->hills)
	{
		_hills_texture_rect.y = _hills_texture_rect.h * hill.type;
		_hills_texture_rect.x = (hill.y_channel == 0) ? 16 : 0;	// Blue version for ground, green version for horizon
		int32_t entity_x_at_camera_x = render::getScreenXForEntityByCameraAndDistance(hill.x*_scaling, _hills_texture_rect.w*_scaling, (int)world->w*_scaling, camera, hill.distance_factor);
		SDL_Rect hill_rect = { entity_x_at_camera_x, _hills_rect.y + y_channel_coords[hill.y_channel], _hills_texture_rect.w * _scaling, _hills_texture_rect.h * _scaling };
		SDL_RenderCopy(sdl_renderer, _hills_texture, &_hills_texture_rect, &hill_rect);
	}
}

RadarSprite::RadarSprite(Renderer* renderer)
{
	_scaling = renderer->scaling;
	_radar_rect = { 64 * (int32_t)_scaling, 156 * (int32_t)_scaling, 128 * (int32_t)_scaling, 36 * (int32_t)_scaling };
	_view_points = { {
		{ 56 * (int32_t)_scaling, 0 * (int32_t)_scaling },
		{ 70 * (int32_t)_scaling, 0 * (int32_t)_scaling },
		{ 56 * (int32_t)_scaling, 34 * (int32_t)_scaling },
		{ 70 * (int32_t)_scaling, 34 * (int32_t)_scaling }
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

int32_t render::getScreenXForEntityByCameraAndDistance(double entity_x_at_zero, uint32_t entity_sprite_width, uint32_t world_width, const Camera& camera, double distance_factor)
{
	double camera_x_abs = camera.view_rect.x;

	double camera_x_rel;
	if (camera_x_abs > world_width / 2)
		camera_x_rel = camera_x_abs - world_width;
	else
		camera_x_rel = camera_x_abs;

	double entity_x_at_camera_x = entity_x_at_zero - (camera_x_rel * distance_factor);

	if (entity_x_at_camera_x + entity_sprite_width > world_width * distance_factor)
		entity_x_at_camera_x -= world_width * distance_factor;

	return (int32_t)entity_x_at_camera_x;
}