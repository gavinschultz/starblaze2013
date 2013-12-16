#include "stdafx.h"
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <unordered_map>
#include "program.h"

namespace
{
	std::unordered_map<std::string, std::string> items;
}

void debug::console(std::initializer_list<std::string> args)
{
	std::ostringstream ss;
	for (auto& s : args)
	{
		ss << s;
	}
	ss << '\n';
	std::cerr << ss.str();
	OutputDebugString(ss.str().c_str());
}

void debug::set(std::string label, std::string value)
{
	items[label] = value;
}

void debug::set(std::string label, double value) { debug::set(label, std::to_string(value)); }
void debug::set(std::string label, int value) { debug::set(label, std::to_string(value)); }
void debug::set(std::string label, unsigned int value) { debug::set(label, std::to_string(value)); }

const std::unordered_map<std::string, std::string>& debug::getItems()
{
	return items;
}