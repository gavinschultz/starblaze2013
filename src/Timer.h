#pragma once
#include <stdint.h>
#include <memory>

class Timer
{
private:
	class impl; std::unique_ptr<impl> pi;	
public:
	Timer(const int fps = 60);
	~Timer();
	void startFrame();
	void endFrame();
	double getFrameRate() const;
	double getAvgFrameRate() const;
	double getLastFrameDuration() const;
	unsigned int getTotalFrames() const;
	double getTime() const;
	double getFrameStartTime() const;
};

extern std::unique_ptr<Timer> timer; // TODO: if we never have use for any other instance of timer, maybe convert this to a namespace