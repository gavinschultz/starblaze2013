#include "stdafx.h"
#include "Util.h"
#include <math.h>
#include <cmath>

bool util::isRightOf(double x, double y, double width)
{
	if ((x - y) >= 0.0)
		return std::fmod((x - y), width) < width / 2;
	else
		return std::fmod((y - x), width) > width / 2;
}

bool util::isLeftOf(double x, double y, double width) {
	return !isRightOf(x, y, width);
}

double util::relativeX(double reference_x, double target_x, double width)
{
	if (isLeftOf(reference_x, target_x, width))
		return util::abswrap(target_x - reference_x, width);
	else
		return -util::abswrap(reference_x - target_x, width);
}

