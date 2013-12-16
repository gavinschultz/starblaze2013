#pragma once
#include "Phys.h"

namespace util
{
	bool isRightOf(double x, double y, double width);
	bool isLeftOf(double x, double y, double width);
	double relativeX(double reference_x, double target_x, double limit);
}