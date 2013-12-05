#include "stdafx.h"
#include "Phys.h"
#include "Collidable.h"
#include "Entity.h"
#include "Util.h"

NormalCollidable::NormalCollidable(const Rect& outer_box, const std::vector<Rect>& collision_boxes, const std::vector<const std::type_info*>& collidable_types)
: _collidable_types{ collidable_types }, _outer_box{ outer_box }, _collision_boxes{ collision_boxes }
{}

void NormalCollidable::updateCollisionBoxes(Point2D pos, bool hflip = false)
{
	this->reset();
	const auto& base_rects = this->getBaseCollisionBoxes();

	if (!hflip)
	{
		for (unsigned int i = 0; i < base_rects.size(); i++)
		{
			_collision_boxes[i].x = pos.x + base_rects[i].x;
			_collision_boxes[i].y = pos.y + base_rects[i].y;
		}
	}
	else
	{
		// Reverse X positions if flipped
		auto base_rects = getBaseCollisionBoxes();
		for (unsigned int i = 0; i < base_rects.size(); i++)
		{
			_collision_boxes[i].x = pos.x + (_outer_box.w - (base_rects[i].x + base_rects[i].w));
			_collision_boxes[i].y = pos.y + base_rects[i].y;
		}
	}
}

bool NormalCollidable::testBoxCollision(Collidable& collidable)
{
	// NOTE: does not check whether the entities that own these collidables can actually collide, check this separately before calling

	if (!util::areRectanglesIntersecting(_outer_box, collidable.getOuterBox()))
		return false;

	for (auto& a : _collision_boxes)
	{
		for (auto& b : collidable.getCollisionBoxes())
		{
			if (util::areRectanglesIntersecting(a, b))
			{
				this->setCollided();
				collidable.setCollided();
				return true;
			}
		}
	}

	return false;
}