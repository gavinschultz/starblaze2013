#pragma once
#include <memory>
#include "phys.h"

class Camera
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	Camera(const SDL_Rect& window_rect, const SDL_Rect& focus_rect);
	~Camera();
	void update();
	SDL_Rect getViewRect() const;
	SDL_Rect getFocusRect() const;
};