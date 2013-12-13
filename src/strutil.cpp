#include "stdafx.h"
#include <string>
#include <algorithm>
#include "strutil.h"

std::string strutil::strip(const std::string& source, const std::string& charsToRemove)
{
	std::string s = source;
	for (auto c : charsToRemove)
	{
		s.erase(std::remove(s.begin(), s.end(), c), s.end());
	}
	return s;
}