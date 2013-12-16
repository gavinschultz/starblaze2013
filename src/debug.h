#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace debug
{
	class DebugItem
	{
	public:
		std::string label;
		std::string value;
	};

	void console(std::initializer_list<std::string> args);

	// TODO: templates
	void set(std::string label, int value);
	void set(std::string label, unsigned int value);
	void set(std::string label, double value);
	void set(std::string label, std::string value);
	const std::unordered_map<std::string, debug::DebugItem>& getItems();
}