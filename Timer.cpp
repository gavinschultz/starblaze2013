#include "timer.h"
#include <sstream>
#include <Windows.h>

Timer::Timer(const int fps)
{
	this->_fps = fps;
	this->_start_time = 0;
	this->_elapsed_time = 0;
	this->_total_time = 0;
	this->_frame = 0;
}

Timer::~Timer()
{
}

void Timer::startFrame()
{
	_start_time = getTime();
	_frame++;
}

void Timer::endFrame()
{
	_elapsed_time = getTime() - _start_time;
	_total_time += _elapsed_time;
}

double Timer::getFrameRate() const
{
	return 1.0 / _elapsed_time;
}

double Timer::getLastFrameDuration() const
{
	return _elapsed_time;
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

uint32_t Timer::getTotalFrames() const
{
	return this->_frame;
}

void Timer::debug()
{
	std::ostringstream ss;
	ss << "{" << _frame << "}\tStart time: " << _start_time << "\tLast frame: " << this->getLastFrameDuration() << "\tFrame rate:" << this->getFrameRate() << "\tTotal time:" << _total_time << "\n";
	OutputDebugString(ss.str().c_str());
}