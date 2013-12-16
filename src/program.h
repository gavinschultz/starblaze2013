#pragma once
#include <initializer_list>
#include <string>
#include <vector>

#include "debug.h"

const enum RetCode
{
	success = 0,
	runtime_error = 1,
	unknown_error = 2,
	sdl_error = 3
};

namespace program
{
	SDL_Thread* input_detect_thread;
	void exit(int retcode, std::initializer_list<std::string> msg);
	void cleanup();
}

namespace usage
{
	void start();
	void collect(std::string label);
	void finish();
	void to_screen();
}