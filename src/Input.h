#pragma once
#include <SDL.h>
#include <memory>

class InputSystem
{
private:
	class impl; std::unique_ptr<impl> pi;
public:
	InputSystem();
	~InputSystem();
	void update();
	void detect();
};