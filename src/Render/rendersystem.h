#pragma once
#include <memory>
#include "system.h"

class SpriteLoader;

class RenderSystem : public System
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	RenderSystem(unsigned int window_width, unsigned int window_height, unsigned int sprite_scale, int render_width);
	~RenderSystem();

	void update();

	void setFullscreen(bool state);
	bool isFullscreen() const;

	std::string getInfo() const;

	const SpriteLoader& getSpriteLoader() const;
};