#define NOMINMAX	// Windows.h has min/max macros which conflict with both the std::numeric_limits<T>::min/max and std::limits::min/max. This will disable the Windows.h versions.
#include <Windows.h>
#include <SDL.h>