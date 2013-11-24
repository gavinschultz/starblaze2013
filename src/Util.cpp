#include "stdafx.h"
#include "Util.h"
#include <math.h>
#include <cmath>

int util::getsign(int value)
{
	if (value == 0)
		return 0;
	else if (value > 0)
		return 1;
	else
		return -1;
}

int util::getsign(double value)
{
	if (value == 0)
		return 0;
	else if (value > 0)
		return 1;
	else
		return -1;
}

double util::round(double value, int sf)
{
	return std::floor((value*std::pow(10, sf)) + 0.5) / std::pow(10, sf);
}

double util::abswrap(double n, double limit)
{
	if (n < 0)
		return limit + std::fmod(n, limit);
	else if (n > limit)
		return std::fmod(n, limit);
	else
		return n;
}

int util::abswrap(int n, int limit)
{
	if (n < 0)
		return limit + (n % limit);
	else if (n > limit)
		return n % limit;
	else
		return n;
}