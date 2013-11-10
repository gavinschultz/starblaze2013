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

Debug::Debug()
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