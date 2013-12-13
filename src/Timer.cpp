#include "stdafx.h"
#include "timer.h"
#include <sstream>
#include <Windows.h>

class Timer::impl
{
public:
	int fps;
	double start_time;
	double elapsed_time;
	double total_time;
	unsigned int frame;
};

Timer::Timer(const int fps) : pi{ new impl{} }
{
	pi->fps = fps;
	pi->start_time = 0;
	pi->elapsed_time = 0;
	pi->total_time = 0;
	pi->frame = 0;
}

Timer::~Timer()
{
}

void Timer::startFrame()
{
	pi->start_time = getTime();
	pi->frame++;
}

void Timer::endFrame()
{
	pi->elapsed_time = getTime() - pi->start_time;
	pi->total_time += pi->elapsed_time;
}

double Timer::getFrameRate() const
{
	return 1.0 / pi->elapsed_time;
}

double Timer::getLastFrameDuration() const
{
	return pi->elapsed_time;
}

// Get time in seconds since the simulation has started
double Timer::getTime() const
{
	static uint64_t start = 0;
	static uint64_t frequency = 0;
	uint64_t counter;
	if (start == 0)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&start);
		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
		return 0;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&counter);

	return ((double)(counter - start) / (double)frequency);
}

unsigned int Timer::getTotalFrames() const
{
	return pi->frame;
}

double Timer::getFrameStartTime() const
{
	return pi->start_time;
}