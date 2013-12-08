#include "stdafx.h"
#include <vector>
#include <cstdio>
#include "program.h"
#include "timer.h"
#include "mathutil.h"

class UsagePoint
{
public:
	std::string label;
	double time1;
	double time2;
	double getUsagePercentage(double frame_time)
	{
		return mathutil::round(((time2 - time1) / frame_time), 4) * 100.0;
	}
};

namespace
{
	std::vector<UsagePoint> usage_points;
	double frame_duration;
}

void usage::start()
{
	usage_points.clear();
	frame_duration = 0.0;
}

void usage::collect(std::string label)
{
	double time1, time2;
	if (usage_points.size() == 0)
		time1 = timer->getFrameStartTime();
	else
		time1 = usage_points.back().time2;
	time2 = timer->getTime();
	usage_points.push_back({ label, time1, time2 });
}

void usage::finish()
{
	frame_duration = timer->getLastFrameDuration();
}

void usage::to_screen()
{
	if (timer->getTotalFrames() % 10 != 0)
		return;

	for (auto& up : usage_points)
	{
		char buffer[10];
		std::sprintf(buffer, "%3.2f", up.getUsagePercentage(frame_duration));
		debug::set("Usage (" + up.label + ") %", buffer);
	}
}