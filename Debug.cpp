#include "Debug.h"
#include <string>
#include <Windows.h>
#include <stdarg.h>
#include <iostream>

void console_debug(std::initializer_list<std::string> args)
{
	std::ostringstream ss;
	for (auto& s : args)
	{
		std::cerr << s;
		ss << s;
	}
	std::cerr << '\n';
	ss << '\n';
	OutputDebugString(ss.str().c_str());
}

Debug::Debug() : motion_history(motion_history_limit, Vector2D{0.0,0.0})
{
	
}

Debug::~Debug()
{

}

void Debug::set(std::string label, std::string value)
{
	for (auto& i : items)
	{
		if (i->label == label)
		{
			i->value = value;
			return;
		}
	}

	auto item = new DebugItem{ label, value };
	items.push_back(std::unique_ptr<DebugItem>{item});
}

void Debug::set(std::string label, double value)
{
	this->set(label, std::to_string(value));
}

void Debug::set(std::string label, int value)
{
	this->set(label, std::to_string(value));
}

void Debug::setMotionRecordMaxThresholds(double x, double y)
{
	motion_max_thresholds = Vector2D{ x, y };
}

void Debug::addMotionRecord(double x, double y)
{
	if (motion_history_counter >= motion_history.size()-1)
		motion_history_counter = 0;
	else
		motion_history_counter++;
	console_debug({ "motion history[", std::to_string(motion_history_counter), "] = (", std::to_string(x), ", ", std::to_string(y), ")" });
	motion_history[motion_history_counter] = Vector2D{ x, y };
}