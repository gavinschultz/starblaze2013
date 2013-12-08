#pragma once
#include <initializer_list>
#include <string>
#include <vector>

const enum RetCode
{
	success = 0,
	runtime_error = 1,
	unknown_error = 2,
	sdl_error = 3
};

namespace program
{
	void exit(int retcode, std::initializer_list<std::string> msg);
	void cleanup();
}

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
	void set(std::string label, double value);
	void set(std::string label, std::string value);
	const std::vector<DebugItem>& getItems();
}

namespace usage
{
	void start();
	void collect(std::string label);
	void finish();
	void to_screen();
}