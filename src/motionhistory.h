#pragma once

namespace motionhistory
{
	void init(unsigned int max_data_points, float threshold_x, float threshold_y);
	//void setMotionRecordMaxThresholds(float x, float y);
	void add(float x, float y);
	const std::vector<Vector2D>& get();
	Vector2D getThresholds();
	unsigned int getCurrentIndex();
}