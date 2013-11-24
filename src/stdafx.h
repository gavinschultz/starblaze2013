#define _CRT_SECURE_NO_WARNINGS // Prevent warnings for using some standard library funcs over safer (but possibly non-portable) windows versions e.g. fopen_s() vs fopen().
#define NOMINMAX	// Windows.h has min/max macros which conflict with both the std::numeric_limits<T>::min/max and std::limits::min/max. This will disable the Windows.h versions.
#include <Windows.h>
#include <SDL.h>