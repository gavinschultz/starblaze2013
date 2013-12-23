#pragma once

namespace motionhistory
{
	void init(unsigned int max_data_points, double threshold_x, double threshold_y);
	//void setMotionRecordMaxThresholds(double x, double y);
	void add(double x, double y);
	const std::vector<Vector2D>& get();
	Vector2D getThresholds();
	unsigned int getCurrentIndex();
}