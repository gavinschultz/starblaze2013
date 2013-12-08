#include "stdafx.h"
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <vector>
#include "program.h"

namespace
{
	std::vector<debug::DebugItem> items;
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
	for (auto& i : items)
	{
		if (i.label == label)
		{
			i.value = value;
			return;
		}
	}
	items.push_back(DebugItem{label, value});
}

void debug::set(std::string label, double value)
{
	debug::set(label, std::to_string(value));
}

void debug::set(std::string label, int value)
{
	debug::set(label, std::to_string(value));
}

const std::vector<debug::DebugItem>& debug::getItems()
{
	if (items.size() > 0)
		debug::console({ "Item: ", items[0].label, "/", items[0].value });
	return items;
}