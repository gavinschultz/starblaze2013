#pragma once
#include "Phys.h"

namespace mathutil
{
	double round(double value, int sf);
	Vector2D normaliseVector(Vector2D v, double* magnitude);
	double abswrap(double n, double limit);
	template <typename T> int getsign(T value);
	//int getsign(int value);
	//int getsign(double value);
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
//int util::getsign(double value)
//{
//	if (value == 0)
//		return 0;
//	else if (value > 0)
//		return 1;
//	else
//		return -1;
//}