#include "stdafx.h"
#include "program.h"

void program::exit(int retcode, std::initializer_list<std::string> msg)
{
	debug::console(msg);
	cleanup();
	std::exit(retcode);
}

void program::cleanup()
{
	debug::console({ "Shutting down..." });
	SDL_Quit();
}