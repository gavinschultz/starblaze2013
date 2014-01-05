#pragma once
#include "Phys.h"

namespace mathutil
{
	float round(float value, int sf);
	Vector2D normaliseVector(Vector2D v, float* magnitude);
	float abswrap(float n, float limit);
	template <typename T> int getsign(T value);
	bool areRectanglesIntersecting(const Rect& a, const Rect& b);
	//int getsign(int value);
	//int getsign(float value);
}

template <typename T>
int mathutil::getsign(T value)
{
	if (value == 0)
		return 0;
	else if (value > 0)
		return 1;
	else
		return -1;
}
//
//int util::getsign(int value)
//{
//	if (value == 0)
//		return 0;
//	else if (value > 0)
//		return 1;
//	else
//		return -1;
//}
//
//int util::getsign(float value)
//{
//	if (value == 0)
//		return 0;
//	else if (value > 0)
//		return 1;
//	else
//		return -1;
//}