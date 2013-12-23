#pragma once
#include "component_base.h"
#include "phys.h"

class TemporalState2DComponent;
class HorizontalOrientComponent;

class CollisionComponent : public Component
{
public:
	CollisionComponent(Rect base_outer_box, std::initializer_list<Rect> base_inner_boxes) : Component(C::ccollision),
		base_outer_box{ base_outer_box },
		base_inner_boxes{ base_inner_boxes },
		outer_box{ base_outer_box },
		inner_boxes{ base_inner_boxes }
	{}
	const Rect base_outer_box;
	const std::vector<Rect> base_inner_boxes;
	Rect outer_box;
	std::vector<Rect> inner_boxes;
	void updateCollisionBoxes(TemporalState2DComponent* state, HorizontalOrientComponent* horient);
};