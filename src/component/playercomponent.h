#pragma once
#include "component_base.h"

class PlayerComponent : public Component
{
public:
	PlayerComponent() : Component(C::player) {}
	unsigned int lives{ 0 };
	unsigned int bullet_count{ 255 };
	unsigned int shields{ 255 };
	unsigned int radar{ 255 };
	unsigned int fuel{ 255 };
};