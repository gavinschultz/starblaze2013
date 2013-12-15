#include "stdafx.h"
#include "mathutil.h"
#include <cmath>

Vector2D mathutil::normaliseVector(Vector2D v, double* magnitude)
{
	double m = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
	Vector2D v_out = { v.x / m, v.y / m };
	if (magnitude)
		*magnitude = m;
	return v_out;
}

double mathutil::round(double value, int sf)
{
	return std::floor((value*std::pow(10, sf)) + 0.5) / std::pow(10, sf);
}

double mathutil::abswrap(double n, double limit)
{
	if (n < 0)
		return limit + std::fmod(n, limit);
	else if (n > limit)
		return std::fmod(n, limit);
	else
		return n;
}