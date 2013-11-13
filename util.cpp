#include "Util.h"

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