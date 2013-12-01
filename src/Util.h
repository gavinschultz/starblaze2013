#pragma once
#include "Phys.h"

namespace util
{
	//TODO: convert to template
	int getsign(int value);
	int getsign(double value);

	double round(double value, int sf);

	//TODO: convert to template
	double abswrap(double n, double limit);
	int abswrap(int n, int limit);

	bool isRightOf(double x, double y, double width);
	bool isLeftOf(double x, double y, double width);
	double relativeX(double reference_x, double target_x, double limit);

	bool areRectanglesIntersecting(const Rect& a, const Rect& b);
	bool areRectanglesIntersecting(const SDL_Rect& a, const SDL_Rect& b);
}