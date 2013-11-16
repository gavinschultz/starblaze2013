#pragma once
#include "Phys.h"
#include <sstream>
#include <string>
#include <unordered_map>
#include <memory>

void console_debug(std::initializer_list<std::string> args);

class DebugItem
{
public:
	std::string label;
	std::string value;
};

class Debug
{
private:
	const unsigned int motion_history_limit{ 1024 };
public:
	std::vector<std::unique_ptr<DebugItem>> items;
	std::vector<Vector2D> motion_history;
	unsigned int motion_history_counter{ 0 };
	Vector2D motion_max_thresholds{ Vector2D{ 0.0, 0.0 } };

	Debug();
	~Debug();

	//TODO: use templates for this instead
	void set(std::string label, int value);
	void set(std::string label, double value);
	void set(std::string label, std::string value);

	void addMotionRecord(double x, double y);
	void setMotionRecordMaxThresholds(double x, double y);
};

extern std::unique_ptr<Debug> debug;