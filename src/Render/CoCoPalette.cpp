#include "stdafx.h"
#include <array>

/*
The TRS-80 CoCo uses the Motorola 6847 VDG.

+ * GREEN = Y'UV(153,-128,-128) = RGB(7,277,-107) --> #07ff00
+ * YELLOW = Y'UV(255,-128,0) = RGB(255,305,-5) --> #ffff00
+ * BLUE = Y'UV(59,+128,0) = RGB(59,8,319) --> #3b08ff
+ * RED: Y'UV(59,0,+128) = RGB(204,-15,59) --> #cc003b
+ * WHITE: Y'UV(255,0,0) = RGB(255,255,255) --> #ffffff
+ * CYAN: Y'UV(153,0,-128) = RGB(7,227,153) --> #07e399
+ * MAGENTA: Y'UV(153,+128,+128) = RGB(298,28,413) --> #ff1cff
+ * ORANGE: Y'UV(153,-128,+128) = RGB(298,129,-107) --> #ff8100
+ * BLACK: Y'UV(0,0,0) = RGB(0,0,0) --> #000000
+ * DARK GREEN: Y'UV(0,-128,-128) = RGB(-145,124,-260) --> #007c00
+ * DARK ORANGE: Y'UV(0,-128,+128) = RGB(145,-23,-260) --> #910000

From http://forums.bannister.org/ubbthreads.php?ubb=showflat&Number=64986&PHPSESSID=3c74d06c904029be10e5bfea6062aeab
*/

namespace CoCoPalette
{
	std::array<SDL_Color, 11> colors { {
		SDL_Color{ 7, 255, 0, 255 },
		SDL_Color{ 255, 255, 0, 255 },
		SDL_Color{ 59, 8, 255, 255 },
		SDL_Color{ 204, 0, 59, 255 },
		SDL_Color{ 255, 255, 255, 255 },
		SDL_Color{ 7, 227, 153, 255 },
		SDL_Color{ 255, 28, 255, 255 },
		SDL_Color{ 255, 129, 0, 255 },
		SDL_Color{ 0, 0, 0, 255 },
		SDL_Color{ 0, 124, 0, 255 },
		SDL_Color{ 145, 0, 0, 255 }
	} };
}