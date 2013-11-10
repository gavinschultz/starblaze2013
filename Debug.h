#pragma once
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
public:
	std::vector<std::unique_ptr<DebugItem>> items;

	Debug();

	//TODO: use templates for this instead
	void set(std::string label, int value);
	void set(std::string label, double value);
	void set(std::string label, std::string value);
};

extern std::unique_ptr<Debug> debug;