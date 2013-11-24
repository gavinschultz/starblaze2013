#pragma once

namespace util
{
	//TODO: convert to template
	int getsign(int value);
	int getsign(double value);

	double round(double value, int sf);

	//TODO: convert to template
	double abswrap(double n, double limit);
	int abswrap(int n, int limit);
}