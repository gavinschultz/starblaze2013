#include "stdafx.h"
#include "collisioncomponent.h"
#include "temporalstatecomponent.h"
#include "horizontalorientcomponent.h"

const Point2Di CollisionComponent::sector_max{ 64, 8 };

void CollisionComponent::updateCollisionBoxes(TemporalState2DComponent* state, HorizontalOrientComponent* horient, float playfield_width, float playfield_height)
{
	outer_box.x = state->current.pos.x;
	outer_box.y = state->current.pos.y;

	if (!horient || horient->direction == HOrient::right)
	{
		for (unsigned int i = 0; i < base_inner_boxes.size(); i++)
		{
			inner_boxes[i].x = state->current.pos.x + base_inner_boxes[i].x;
			inner_boxes[i].y = state->current.pos.y + base_inner_boxes[i].y;
		}
	}
	else
	{
		for (unsigned int i = 0; i < base_inner_boxes.size(); i++)
		{
			inner_boxes[i].x = state->current.pos.x + (outer_box.w - (base_inner_boxes[i].x + base_inner_boxes[i].w));
			inner_boxes[i].y = state->current.pos.y + base_inner_boxes[i].y;
		}
	}

	sector.x = std::lround((state->current.pos.x / playfield_width) * sector_max.x) % sector_max.x;
	sector.y = (int)((state->current.pos.y / playfield_height) * sector_max.y);
}