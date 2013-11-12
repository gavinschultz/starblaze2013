#include "Game.h"
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include "Debug.h"

Game::Game()
{

}

Game::~Game()
{

}

void Game::start()
{
	
}

void Game::togglePause(bool state)
{
	is_paused = state;
	if (!is_paused)
		is_frame_by_frame = false;
}

void Game::advanceFrameByFrame()
{
	is_frame_by_frame = true;
	is_paused = true;
}