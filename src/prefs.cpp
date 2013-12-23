#include "stdafx.h"
#include "prefs.h"

#ifdef _DEBUG
bool prefs::show_fps = true;
bool prefs::show_debug = true;
bool prefs::show_grid = true;
bool prefs::show_collision_boxes = true;
#else
bool prefs::show_fps = false;
bool prefs::show_debug = false;
bool prefs::show_grid = false;
bool prefs::show_collision_boxes = false;
#endif
bool prefs::show_motionhistory = false;
bool prefs::smooth_animation = false;
bool prefs::vsync = true;