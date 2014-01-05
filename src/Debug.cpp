#include "stdafx.h"
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <map>
#include "program.h"

namespace
{
	std::map<std::string, std::string> items;
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
	//if (label.find("render") != std::string::npos)	// testing only
	items[label] = value;
}

void debug::set(std::string label, float value) { debug::set(label, std::to_string(value)); }
void debug::set(std::string label, int value) { debug::set(label, std::to_string(value)); }
void debug::set(std::string label, unsigned int value) { debug::set(label, std::to_string(value)); }

const std::map<std::string, std::string>& debug::getItems()
{
	return items;
}