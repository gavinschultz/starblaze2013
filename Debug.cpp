#include "Debug.h"
#include <string>
#include <Windows.h>
#include <stdarg.h>
#include <iostream>

void debug(std::initializer_list<std::string> args)
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