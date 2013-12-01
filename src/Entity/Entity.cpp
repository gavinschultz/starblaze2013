#include "stdafx.h"
#include "Entity.h"

void Entity::updateCollisionBoxes()
{
	const auto* base_rects = this->getBaseCollisionBoxes();
	if (!base_rects)
		return;
	
	for (unsigned int i = 0; i < base_rects->size(); i++)
	{
		collision_boxes[i].x = current_state.pos.x + (*base_rects)[i].x;
		collision_boxes[i].y = current_state.pos.y + (*base_rects)[i].y;
	}
}