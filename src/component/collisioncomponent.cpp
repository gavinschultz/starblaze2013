#include "stdafx.h"
#include "collisioncomponent.h"
#include "temporalstatecomponent.h"
#include "horizontalorientcomponent.h"

void CollisionComponent::updateCollisionBoxes(TemporalState2DComponent* state, HorizontalOrientComponent* horient)
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
}