#include "stdafx.h"
#include "MathUtil.h"
#include <cmath>

Vector2D mathutil::normaliseVector(Vector2D v, double* magnitude)
{
	double m = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
	Vector2D v_out = { v.x / m, v.y / m };
	if (magnitude)
		*magnitude = m;
	return v_out;
}