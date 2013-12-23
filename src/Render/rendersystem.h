#pragma once
#include <memory>
#include <glew.h>
#include "system.h"
#include "phys.h"

class SpriteLoader;
class Camera;

class RenderSystem : public System
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	RenderSystem(Window window);
	~RenderSystem();

	SDL_Palette* palette;

	void init();
	void draw(Camera& camera);

	void setFullscreen(bool state);
	bool isFullscreen() const;

	std::string getInfo() const;
	const Window& getWindow() const;

	const SpriteLoader& getSpriteLoader() const;
};

extern std::unique_ptr<RenderSystem> renderer;