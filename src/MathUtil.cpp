#include "stdafx.h"
#include "mathutil.h"
#include <cmath>

Vector2D mathutil::normaliseVector(Vector2D v, float* magnitude)
{
	float m = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
	Vector2D v_out = { v.x / m, v.y / m };
	if (magnitude)
		*magnitude = m;
	return v_out;
}

float mathutil::round(float value, int sf)
{
	return std::floor((value*std::pow(10, sf)) + 0.5) / std::pow(10, sf);
}

float mathutil::abswrap(float n, float limit)
{
	if (n < 0)
		return limit + std::fmod(n, limit);
	else if (n > limit)
		return std::fmod(n, limit);
	else
		return n;
}

int mathutil::abswrap(int n, int limit)
{
	if (n < 0)
		return limit + (n % limit);
	else if (n > limit)
		return n % limit;
	else
		return n;
}

//TODO: make templates
bool mathutil::areRectanglesIntersecting(const Rect& a, const Rect& b)
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
//bool mathutil::areRectanglesIntersecting(const SDL_Rect& a, const SDL_Rect& b)
//{
//	if (a.x + a.w < b.x)
//		return false;
//	if (b.x + b.w < a.x)
//		return false;
//	if (a.y + a.h < b.y)
//		return false;
//	if (b.y + b.h < a.y)
//		return false;
//	return true;
//	//return SDL_HasIntersection(&a, &b);
//}