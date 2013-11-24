#include "stdafx.h"
#include "RenderUtil.h"
#include <cmath>

int32_t renderutil::getScreenXForEntityByCameraAndDistance(double entity_x_at_zero, uint32_t entity_sprite_width, uint32_t world_width, const Camera& camera, double distance_factor)
{
	double camera_x_abs = camera.view_rect.x;

	double camera_x_rel;
	if (camera_x_abs > world_width / 2)
		camera_x_rel = camera_x_abs - world_width;
	else
		camera_x_rel = camera_x_abs;

	double entity_x_at_camera_x = entity_x_at_zero - (camera_x_rel * distance_factor);

	if (entity_x_at_camera_x + entity_sprite_width > world_width * distance_factor)
		entity_x_at_camera_x -= world_width * distance_factor;

	return std::lround(entity_x_at_camera_x);
}