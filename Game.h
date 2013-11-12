#pragma once
#include <memory>
#include "Entity.h"
#include "Renderer.h"

class Game
{
public:
	Game();
	~Game();
	void start();
	void togglePause(bool state);
	bool is_paused{ false };
	void advanceFrameByFrame();
	bool is_frame_by_frame{ false };
	EntityRegister entity_register;
	SDL_Rect ship_limits;
	float mouse_sensitivity{ 3.5 };
};

extern std::unique_ptr<Game> game;