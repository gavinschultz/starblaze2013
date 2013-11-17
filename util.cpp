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