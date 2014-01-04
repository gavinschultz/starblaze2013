#define _CRT_SECURE_NO_WARNINGS // Prevent warnings for using some standard library funcs over safer (but possibly non-portable) windows versions e.g. fopen_s() vs fopen().
#define NOMINMAX	// Windows.h has min/max macros which conflict with both the std::numeric_limits<T>::min/max and std::limits::min/max. This will disable the Windows.h versions.
#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0

#include <Windows.h>
#include <SDL.h>
//#include "entity.h"
//#include "component.h"
//#include "system.h"
#include "program.h"