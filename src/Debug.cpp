#include "stdafx.h"
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <unordered_map>
#include "program.h"

namespace
{
	std::unordered_map<std::string, debug::DebugItem> items;
}

void debug::console(std::initializer_list<std::string> args)
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

void debug::set(std::string label, std::string value)
{
	items[label].label = label;
	items[label].value = value;
	//for (auto& i : items)
	//{
	//	if (i.label == label)
	//	{
	//		i.value = value;
	//		return;
	//	}
	//}
	//items.push_back(DebugItem{label, value});
}

void debug::set(std::string label, double value)
{
	debug::set(label, std::to_string(value));
}

void debug::set(std::string label, int value)
{
	debug::set(label, std::to_string(value));
}

void debug::set(std::string label, unsigned int value)
{
	debug::set(label, std::to_string(value));
}

const std::unordered_map<std::string, debug::DebugItem>& debug::getItems()
{
	return items;
}