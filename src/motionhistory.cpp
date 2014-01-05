#include "stdafx.h"
#include <vector>
#include "phys.h"
#include "motionhistory.h"

namespace
{
	unsigned int motion_history_limit{ 1024 };
	std::vector<Vector2D> motion_history;
	unsigned int motion_history_counter{ 0 };
	Vector2D motion_max_thresholds{ Vector2D{ 0.0, 0.0 } };
}

void motionhistory::init(unsigned int max_data_points, float threshold_x, float threshold_y)
{
	motion_history_limit = max_data_points;
	motion_history = std::vector<Vector2D>(motion_history_limit);
	motion_max_thresholds = Vector2D{ threshold_x, threshold_y };
}
//
//void motionhistory::setMotionRecordMaxThresholds(float x, float y)
//{
//	motion_max_thresholds = Vector2D{ x, y };
//}

void motionhistory::add(float x, float y)
{
	if (motion_history_counter >= motion_history.size() - 1)
		motion_history_counter = 0;
	else
		motion_history_counter++;
	motion_history[motion_history_counter] = Vector2D{ x, y };
	//debug::console({ "history ", std::to_string(motion_history_counter), ": ", std::to_string(motion_history[motion_history_counter].x), ", ", std::to_string(motion_history[motion_history_counter].y) });
}

const std::vector<Vector2D>& motionhistory::get()
{
	return motion_history;
}

Vector2D motionhistory::getThresholds()
{
	return motion_max_thresholds;
}

unsigned int motionhistory::getCurrentIndex()
{
	return motion_history_counter;
}