#pragma once
#include <stdint.h>

class Timer
{
private:
	int _fps;
	double _start_time;
	double _elapsed_time;
	double _total_time;
	uint32_t _frame;
	double getTime() const;
	void debug();
public:
	Timer(const int fps = 60);
	~Timer();
	void startFrame();
	void endFrame();
	double getFrameRate() const;
	double getLastFrameDuration() const;
	uint32_t getTotalFrames() const;
};

