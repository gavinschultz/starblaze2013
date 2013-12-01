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

//TODO: make templates
bool util::areRectanglesIntersecting(const Rect& a, const Rect& b)
{
	if (a.x + a.w < b.x)
		return false;
	if (b.x + b.w < a.x)
		return false;
	if (a.y + a.h < b.y)
		return false;
	if (b.y + b.h < a.y)
		return false;
	return true;
}
bool util::areRectanglesIntersecting(const SDL_Rect& a, const SDL_Rect& b)
{
	if (a.x + a.w < b.x)
		return false;
	if (b.x + b.w < a.x)
		return false;
	if (a.y + a.h < b.y)
		return false;
	if (b.y + b.h < a.y)
		return false;
	return true;
	//return SDL_HasIntersection(&a, &b);
}