#pragma once
#include "Phys.h"

namespace mathutil
{
	double round(double value, int sf);
	Vector2D normaliseVector(Vector2D v, double* magnitude);
	double abswrap(double n, double limit);
}