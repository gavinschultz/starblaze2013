#pragma once
#include <memory>
#include "system.h"
#include "phys.h"

class SpriteLoader;
class Camera;

class RenderSystem : public System
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	RenderSystem(unsigned int window_width, unsigned int window_height, unsigned int sprite_scale, int render_width);
	~RenderSystem();

	SDL_Palette* palette;

	void draw(Camera& camera);

	void setFullscreen(bool state);
	bool isFullscreen() const;

	std::string getInfo() const;
	const Window& getWindow() const;

	const SpriteLoader& getSpriteLoader() const;
};