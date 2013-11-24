#pragma once
#include "Camera.h"

namespace renderutil
{
	int32_t getScreenXForEntityByCameraAndDistance(double entity_x_at_zero, uint32_t entity_sprite_width, uint32_t world_width, const Camera& camera, double distance_factor);
}